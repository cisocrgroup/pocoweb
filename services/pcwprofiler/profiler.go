package main

import (
	"compress/gzip"
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/finkf/gofiler"
	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type profiler struct {
	project *db.Project
	alex    api.AdditionalLexicon
	config  gofiler.LanguageConfiguration
	profile gofiler.Profile
	types   map[string]int
}

func (p profiler) BookID() int {
	return p.project.BookID
}

func (p profiler) Name() string {
	if len(p.alex.Tokens) == 0 {
		return "profiling"
	}
	return "profiling with extended lexicon"
}

func (p *profiler) Run(ctx context.Context) error {
	log.Debug("profiler: run()")
	if err := p.findLanguage(); err != nil {
		return fmt.Errorf("cannot profile: %v", err)
	}
	if err := p.runProfiler(ctx); err != nil {
		return fmt.Errorf("cannot profile: %v", err)
	}
	if err := p.writeProfile(); err != nil {
		return fmt.Errorf("cannot profile: %v", err)
	}
	if err := p.insertProfileIntoDB(); err != nil {
		return fmt.Errorf("cannot profile: %v", err)
	}
	return nil
}

func (p *profiler) findLanguage() error {
	log.Debug("profiler: findLanguage()")
	config, err := gofiler.FindLanguage(languageDir, p.project.Lang)
	if err != nil {
		// if err == gofiler.ErrorLanguageNotFound could be handled
		// like http.StatusNotFound but we ignore this for now.
		return fmt.Errorf("cannot find language %s: %v", p.project.Lang, err)
	}
	log.Debugf("profiler: found language: %s: %s", config.Language, config.Path)
	p.config = config
	return nil
}

func (p *profiler) runProfiler(ctx context.Context) error {
	log.Debug("profiler: runProfiler()")
	var tokens []gofiler.Token
	for _, token := range p.alex.Tokens {
		tokens = append(tokens, gofiler.Token{LE: token})
	}
	err := eachLine(p.project.BookID, func(line db.Chars) error {
		return eachWord(line, func(word db.Chars) error {
			tokens = append(tokens, gofiler.Token{OCR: word.OCR()})
			if word.IsFullyCorrected() {
				tokens[len(tokens)-1].COR = word.Cor()
			}
			return nil
		})
	})
	if err != nil {
		return fmt.Errorf("cannot run profiler: %v", err)
	}
	log.Debugf("profiler: profiling %d tokens", len(tokens))
	profile, err := gofiler.Run(ctx, profbin, p.config.Path, tokens, logger{})
	if err != nil {
		return fmt.Errorf("cannot run profiler: %v", err)
	}
	log.Debugf("profiler: got %d profile entries", len(profile))
	p.profile = profile
	return nil
}

func (p *profiler) writeProfile() (err error) {
	log.Debug("profiler: writeProfile()")
	dest := filepath.Join(baseDir, p.project.Directory, "profile.json.gz")
	out, err := os.Create(dest)
	if err != nil {
		return fmt.Errorf("cannot write profile %s: %v", dest, err)
	}
	defer func() {
		if xerr := out.Close(); xerr != nil && err == nil {
			err = fmt.Errorf("cannot write profile %s: %v", dest, xerr)
		}
	}()
	gzip := gzip.NewWriter(out)
	defer func() {
		if xerr := gzip.Close(); xerr != nil && err == nil {
			err = fmt.Errorf("cannot write profile %s: %v", dest, xerr)
		}
	}()
	if err := json.NewEncoder(gzip).Encode(p.profile); err != nil {
		return fmt.Errorf("cannot write profile %s: %v", dest, err)
	}
	return nil
}

func (p *profiler) insertProfileIntoDB() error {
	log.Debug("profiler: insertProfileIntoDB()")
	t := db.NewTransaction(service.Pool().Begin())
	p.types = make(map[string]int)
	t.Do(func(dtb db.DB) error {
		if err := p.deleteProfile(dtb); err != nil {
			return fmt.Errorf("cannot insert profile: %v", err)
		}
		inserter := profileInserter{
			profile: p.profile,
			types:   make(map[string]int),
			bid:     p.project.BookID,
		}

		if err := inserter.insert(dtb); err != nil {
			return fmt.Errorf("cannot insert profile: %v", err)
		}
		stmnt := "UPDATE books SET profiled=? WHERE bookid=?"
		if _, err := db.Exec(dtb, stmnt, true, p.project.BookID); err != nil {
			return fmt.Errorf("cannot insert profile: %v", err)
		}
		return nil
	})
	return t.Done()
}

func (p *profiler) deleteProfile(dtb db.DB) error {
	tables := []string{"suggestions", "errorpatterns", "typcounts"}
	for _, table := range tables {
		stmt := "DELETE FROM " + table + " WHERE bookid=?"
		_, err := db.Exec(dtb, stmt, p.project.BookID)
		if err != nil {
			return fmt.Errorf("cannot delete profile: %v", err)
		}
	}
	return nil
}

type profileInserter struct {
	profile gofiler.Profile
	types   map[string]int
	bid     int
}

func (p *profileInserter) insert(dtb db.DB) error {
	if err := p.insertTypes(dtb); err != nil {
		return fmt.Errorf("cannot insert: %v", err)
	}
	if err := p.insertCandidates(dtb); err != nil {
		return fmt.Errorf("cannot insert: %v", err)
	}
	return nil
}

func (p *profileInserter) insertTypes(dtb db.DB) error {
	// prepare insert statements
	qt, err := dtb.Prepare("SELECT id FROM types WHERE typ=?")
	if err != nil {
		return fmt.Errorf("cannot insert types: %v", err)
	}
	defer qt.Close()
	it, err := dtb.Prepare("INSERT into types (typ) VALUES (?)")
	if err != nil {
		return fmt.Errorf("cannot insert types: %v", err)
	}
	defer it.Close()
	tc, err := dtb.Prepare("INSERT INTO typcounts (typid,bookid,counts) " +
		"VALUES (?,?,?) " +
		"ON DUPLICATE KEY UPDATE counts = counts + ?")
	if err != nil {
		return fmt.Errorf("cannot insert types: %v", err)
	}
	defer tc.Close()
	for _, interp := range p.profile {
		// skip if no candidates
		if len(interp.Candidates) == 0 {
			continue
		}
		// insert interp.OCR p.type[interp.OCR] gives the token id
		if err := p.insertType(qt, it, interp.OCR); err != nil {
			return fmt.Errorf("cannot insert types: %v", err)
		}
		// update counts (p.types must contain id of interp.OCR)
		n := interp.N
		if _, err := tc.Exec(p.types[interp.OCR], p.bid, n, n); err != nil {
			return fmt.Errorf("cannot insert types: %v", err)
		}
		// update candidate Suggestion and modern types
		// if they are not too unlikely
		for _, cand := range interp.Candidates {
			// skip if weight is too low
			if float64(cand.Weight) <= cutoff {
				continue
			}
			if err := p.insertType(qt, it, cand.Suggestion); err != nil {
				return fmt.Errorf("cannot insert types: %v", err)
			}
			if err := p.insertType(qt, it, cand.Modern); err != nil {
				return fmt.Errorf("cannot insert types: %v", err)
			}
		}
	}
	return nil
}

func (p *profileInserter) insertType(qt, it *sql.Stmt, typ string) error {
	if _, ok := p.types[typ]; ok {
		return nil
	}
	var id int
	err := qt.QueryRow(typ).Scan(&id)
	if err != nil && err != sql.ErrNoRows {
		return fmt.Errorf("cannot insert type %q: %v", typ, err)
	}
	if err == nil {
		p.types[typ] = id
		return nil
	}
	res, err := it.Exec(typ)
	if err != nil {
		return fmt.Errorf("cannot insert type %q: %v", typ, err)
	}
	tid, err := res.LastInsertId()
	if err != nil {
		return fmt.Errorf("cannot insert type %q: %v", typ, err)
	}
	p.types[typ] = int(tid)
	return nil
}

func (p *profileInserter) insertCandidates(dtb db.DB) error {
	// prepare insert statements
	stmt := fmt.Sprintf("INSERT INTO %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) "+
		"VALUES(?,?,?,?,?,?,?,?,?,?)",
		db.SuggestionsTableName,
		db.SuggestionsTableBookID,
		db.SuggestionsTableTokenTypeID,
		db.SuggestionsTableSuggestionTypeID,
		db.SuggestionsTableModernTypeID,
		db.SuggestionsTableDict,
		db.SuggestionsTableWeight,
		db.SuggestionsTableDistance,
		db.SuggestionsTableTopSuggestion,
		db.SuggestionsTableHistPatterns,
		db.SuggestionsTableOCRPatterns)
	ic, err := dtb.Prepare(stmt)
	if err != nil {
		return fmt.Errorf("cannot insert candidates: %v", err)
	}
	defer ic.Close()
	ip, err := dtb.Prepare("INSERT INTO errorpatterns " +
		"(suggestionID,bookID,pattern,ocr) " +
		"VALUES (?,?,?,?)")
	if err != nil {
		return fmt.Errorf("cannot insert candidates: %v", err)
	}
	defer ip.Close()
	// p.types must contain all interp.OCR, cand.Suggestion and cand.Modern
	for _, interp := range p.profile {
		tid := p.types[interp.OCR]
		for i, cand := range interp.Candidates {
			// skip if weight is too low
			if float64(cand.Weight) <= cutoff {
				continue
			}
			if err := p.insertCandidate(ic, ip, cand, tid, i == 0); err != nil {
				return fmt.Errorf("cannot insert candidates: %v", err)
			}
		}
	}
	return nil
}

func (p *profileInserter) insertCandidate(
	ic, ip *sql.Stmt, cand gofiler.Candidate, tid int, top bool) error {

	sid := p.types[cand.Suggestion]
	mid := p.types[cand.Modern]
	hp := patternString(cand.HistPatterns)
	op := patternString(cand.OCRPatterns)

	res, err := ic.Exec(p.bid, tid, sid, mid, cand.Dict,
		cand.Weight, cand.Distance, top, hp, op)
	if err != nil {
		return fmt.Errorf("cannot insert candidate: %v", err)
	}
	cid, err := res.LastInsertId()
	if err != nil {
		return fmt.Errorf("cannot insert candidate: %v", err)
	}
	if err := p.insertPatterns(ip, cand.HistPatterns, int(cid), false); err != nil {
		return fmt.Errorf("cannot insert candidate: %v", err)
	}
	if err := p.insertPatterns(ip, cand.OCRPatterns, int(cid), true); err != nil {
		return fmt.Errorf("cannot insert candidate: %v", err)
	}
	return nil
}

func (p *profileInserter) insertPatterns(
	ip *sql.Stmt, pats []gofiler.Pattern, cid int, ocr bool) error {
	for _, pat := range pats {
		if _, err := ip.Exec(cid, p.bid, pat.Left+":"+pat.Right, ocr); err != nil {
			return fmt.Errorf("cannot insert pattern: %v", err)
		}
	}
	return nil
}

func patternString(ps []gofiler.Pattern) string {
	var strs []string
	for _, p := range ps {
		strs = append(strs, fmt.Sprintf("%s:%s:%d", p.Left, p.Right, p.Pos))
	}
	return strings.Join(strs, ",")
}

func selectBookLines(bookID int) ([]db.Chars, error) {
	const stmt = "SELECT Cor,OCR,LineID FROM " + db.ContentsTableName +
		" WHERE BookID=? ORDER BY PageID, LineID, Seq"
	rows, err := db.Query(service.Pool(), stmt, bookID)
	if err != nil {
		return nil, fmt.Errorf("cannot select lines for book ID %d: %v",
			bookID, err)
	}
	defer rows.Close()
	lineID := -1
	var lines []db.Chars
	for rows.Next() {
		var tmp int
		var char db.Char
		if err := rows.Scan(&char.Cor, &char.OCR, &tmp); err != nil {
			return nil, fmt.Errorf("cannot select lines for book ID %d: %v",
				bookID, err)
		}
		if tmp != lineID {
			lines = append(lines, nil)
			lineID = tmp
		}
		lines[len(lines)-1] = append(lines[len(lines)-1], char)
	}
	return lines, nil
}

func eachLine(bookID int, f func(db.Chars) error) error {
	lines, err := selectBookLines(bookID)
	if err != nil {
		return fmt.Errorf("cannot load lines for book ID %d: %v",
			bookID, err)
	}
	for _, line := range lines {
		if err := f(line); err != nil {
			return err
		}
	}
	return nil
}

func eachWord(line db.Chars, f func(db.Chars) error) error {
	for t, r := line.NextWord(); len(t) > 0; t, r = r.NextWord() {
		if err := f(t); err != nil {
			return err
		}
	}
	return nil
}

type logger struct{}

func (logger) Log(msg string) {
	log.Debug(msg)
}