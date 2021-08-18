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
	"time"
	"unicode"

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

const namePrefix = "profiling"

func (p profiler) Name() string {
	if len(p.alex.Tokens) == 0 {
		return namePrefix
	}
	return namePrefix + " with extended lexicon"
}

func (p *profiler) Run(ctx context.Context) error {
	start := time.Now()
	if p.project.Lang == "" {
		return p.runEmptyLanguage(ctx)
	}
	if err := p.findLanguage(); err != nil {
		return fmt.Errorf("profile: %v", err)
	}
	if err := p.runProfiler(ctx); err != nil {
		return fmt.Errorf("profile: %v", err)
	}
	if err := p.writeProfile(); err != nil {
		return fmt.Errorf("profile: %v", err)
	}
	if err := p.insertProfileIntoDB(); err != nil {
		return fmt.Errorf("profile: %v", err)
	}
	log.Printf("done profiling after %v", time.Since(start))
	return nil
}

func (p *profiler) runEmptyLanguage(ctx context.Context) error {
	const stmnt = "UPDATE books SET profiled=? WHERE bookid=?"
	t := db.NewTransaction(service.Pool().Begin())
	t.Do(func(dtb db.DB) error {
		_, err := db.ExecContext(ctx, dtb, stmnt, true, p.project.BookID)
		return err
	})
	return t.Done()
}

func (p *profiler) findLanguage() error {
	config, err := gofiler.FindLanguage(languageDir, p.project.Lang)
	if err != nil {
		// if err == gofiler.ErrorLanguageNotFound could be handled
		// like http.StatusNotFound but we ignore this for now.
		return fmt.Errorf("find language %s: %v", p.project.Lang, err)
	}
	log.Printf("profiler: found language: %s: %s", config.Language, config.Path)
	p.config = config
	return nil
}

func (p *profiler) runProfiler(ctx context.Context) error {
	var ts []gofiler.Token
	for _, t := range p.alex.Tokens {
		ts = append(ts, gofiler.Token{LE: t})
	}
	err := eachLine(ctx, p.project.BookID, func(line db.Chars) error {
		for w, r := line.NextWord(); len(w) > 0; w, r = r.NextWord() {
			w = trim(w)
			if len(w) < 4 {
				continue
			}
			ts = append(ts, gofiler.Token{OCR: w.OCR()})
			if w.IsManuallyCorrected() {
				ts[len(ts)-1].COR = w.Cor()
			}
		}
		return nil
	})
	if err != nil {
		return fmt.Errorf("run profiler: %v", err)
	}
	x := gofiler.Profiler{
		Exe:      profbin,
		Config:   p.config.Path,
		Log:      logger{},
		Adaptive: true,
		Types:    true,
	}
	log.Printf("profiler: profiling %d tokens", len(ts))
	profile, err := x.Run(ctx, ts)
	if err != nil {
		return fmt.Errorf("run profiler: %v", err)
	}
	p.profile = profile
	log.Printf("got total %d profile entries", len(p.profile))
	return nil
}

func (p *profiler) writeProfile() (err error) {
	dest := filepath.Join(baseDir, p.project.Directory, "profile.json.gz")
	log.Printf("writing profile to %s", dest)
	out, err := os.Create(dest)
	if err != nil {
		return fmt.Errorf("write profile %s: %v", dest, err)
	}
	defer func() {
		if xerr := out.Close(); xerr != nil && err == nil {
			err = fmt.Errorf("write profile %s: %v", dest, xerr)
		}
	}()
	gzip := gzip.NewWriter(out)
	defer func() {
		if xerr := gzip.Close(); xerr != nil && err == nil {
			err = fmt.Errorf("write profile %s: %v", dest, xerr)
		}
	}()
	if err := json.NewEncoder(gzip).Encode(p.profile); err != nil {
		return fmt.Errorf("write profile %s: %v", dest, err)
	}
	return nil
}

func (p *profiler) insertProfileIntoDB() error {
	t := db.NewTransaction(service.Pool().Begin())
	p.types = make(map[string]int)
	t.Do(func(dtb db.DB) error {
		if err := p.deleteProfile(dtb); err != nil {
			return fmt.Errorf("insert profile: %v", err)
		}
		inserter := profileInserter{
			profile: p.profile,
			types:   make(map[string]int),
			bid:     p.project.BookID,
		}

		if err := inserter.insert(dtb); err != nil {
			return fmt.Errorf("insert profile: %v", err)
		}
		stmnt := "UPDATE books SET profiled=? WHERE bookid=?"
		if _, err := db.Exec(dtb, stmnt, true, p.project.BookID); err != nil {
			return fmt.Errorf("insert profile: %v", err)
		}
		return nil
	})
	return t.Done()
}

func (p *profiler) deleteProfile(dtb db.DB) error {
	tables := []string{"errorpatterns", "suggestions", "typcounts"}
	for _, table := range tables {
		stmt := "DELETE FROM " + table + " WHERE bookid=?"
		_, err := db.Exec(dtb, stmt, p.project.BookID)
		if err != nil {
			return fmt.Errorf("delete profile: %v", err)
		}
	}
	return nil
}

type profileInserter struct {
	profile gofiler.Profile
	types   map[string]int
	bid     int
}

func (p *profileInserter) typ(str string) (int, error) {
	id, ok := p.types[str]
	if !ok {
		return 0, fmt.Errorf("missing typeid for %s", str)
	}
	return id, nil
}

func (p *profileInserter) insertType(qt, it *sql.Stmt, typ string) error {
	if _, ok := p.types[typ]; ok {
		return nil
	}
	var id int
	err := qt.QueryRow(typ).Scan(&id)
	if err != nil && err != sql.ErrNoRows {
		return fmt.Errorf("insert type %s: %v", typ, err)
	}
	if err == nil {
		p.types[typ] = id
		return nil
	}
	res, err := it.Exec(typ)
	if err != nil {
		return fmt.Errorf("insert type %s: %v", typ, err)
	}
	tid, err := res.LastInsertId()
	if err != nil {
		return fmt.Errorf("insert type %s: %v", typ, err)
	}
	p.types[typ] = int(tid)
	return nil
}

func (p *profileInserter) insert(dtb db.DB) error {
	if err := p.insertTypes(dtb); err != nil {
		return err
	}
	if err := p.insertCandidates(dtb); err != nil {
		return err
	}
	return nil
}

const none = "__NONE__"

func (p *profileInserter) insertTypes(dtb db.DB) error {
	// prepare insert statements
	qt, err := dtb.Prepare("SELECT id FROM types WHERE typ=?")
	if err != nil {
		return fmt.Errorf("insert types: %v", err)
	}
	defer qt.Close()
	it, err := dtb.Prepare("INSERT into types (typ) VALUES (?)")
	if err != nil {
		return fmt.Errorf("insert types: %v", err)
	}
	defer it.Close()
	tc, err := dtb.Prepare("INSERT INTO typcounts (typid,bookid,counts) " +
		"VALUES (?,?,?) " +
		"ON DUPLICATE KEY UPDATE counts = counts + ?")
	if err != nil {
		return fmt.Errorf("insert types: %v", err)
	}
	defer tc.Close()
	// Insert "none" string into the types database.
	if err := p.insertType(qt, it, none); err != nil {
		return fmt.Errorf("insert types: %v", err)
	}
	for _, interp := range p.profile {
		// insert interp.OCR p.type[interp.OCR] gives the token id
		if err := p.insertType(qt, it, interp.OCR); err != nil {
			return fmt.Errorf("insert types: %v", err)
		}
		// skip if no candidates
		if len(interp.Candidates) == 0 {
			continue
		}
		// update counts (p.types must contain id of interp.OCR)
		n := interp.N
		id, err := p.typ(interp.OCR)
		if err != nil {
			return fmt.Errorf("insert types: %v", err)
		}
		if _, err := tc.Exec(id, p.bid, n, n); err != nil {
			return fmt.Errorf("insert types: %v", err)
		}
		// update candidate Suggestion and modern types
		// if they are not too unlikely
		for _, cand := range interp.Candidates {
			// skip if weight is too low
			if float64(cand.Weight) <= cutoff {
				continue
			}
			if err := p.insertType(qt, it, cand.Suggestion); err != nil {
				return fmt.Errorf("insert types: %v", err)
			}
			if err := p.insertType(qt, it, cand.Modern); err != nil {
				return fmt.Errorf("insert types: %v", err)
			}
		}
	}
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
		return err
	}
	defer ic.Close()
	ip, err := dtb.Prepare("INSERT INTO errorpatterns " +
		"(suggestionID,bookID,pattern,ocr) " +
		"VALUES (?,?,?,?)")
	if err != nil {
		return err
	}
	defer ip.Close()
	// p.types must contain all interp.OCR, cand.Suggestion and cand.Modern
	for _, interp := range p.profile {
		tid, err := p.typ(interp.OCR)
		if err != nil {
			return err
		}
		// insert dummy candidate to handle suspicious words with no candidates
		if len(interp.Candidates) == 0 {
			interp.Candidates = append(interp.Candidates, gofiler.Candidate{
				Suggestion: none,
				Modern:     none,
				Dict:       none,
				Weight:     1.0, // Make sure that the synthetic candidate is not cut off.
			})
		}
		for i, cand := range interp.Candidates {
			// skip if weight is too low
			if float64(cand.Weight) <= cutoff {
				continue
			}
			if err := p.insertCandidate(ic, ip, cand, tid, i == 0); err != nil {
				return err
			}
		}
	}
	return nil
}

func (p *profileInserter) insertCandidate(
	ic, ip *sql.Stmt, cand gofiler.Candidate, tid int, top bool) error {

	sid, err := p.typ(cand.Suggestion)
	if err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	mid, err := p.typ(cand.Modern)
	if err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	hp := patternString(cand.HistPatterns)
	op := patternString(cand.OCRPatterns)
	res, err := ic.Exec(p.bid, tid, sid, mid, cand.Dict,
		cand.Weight, cand.Distance, top, hp, op)
	if err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	cid, err := res.LastInsertId()
	if err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	if err := p.insertPatterns(ip, cand.HistPatterns, int(cid), false); err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	if err := p.insertPatterns(ip, cand.OCRPatterns, int(cid), true); err != nil {
		return fmt.Errorf("insert candidate: %v", err)
	}
	return nil
}

func (p *profileInserter) insertPatterns(
	ip *sql.Stmt, pats []gofiler.Pattern, cid int, ocr bool) error {
	for _, pat := range pats {
		if _, err := ip.Exec(cid, p.bid, pat.Left+":"+pat.Right, ocr); err != nil {
			return fmt.Errorf("insert pattern: %v", err)
		}
	}
	return nil
}

func patternString(ps []gofiler.Pattern) string {
	var strs []string
	for _, p := range ps {
		strs = append(strs, fmt.Sprintf("%s:%s:%d", p.Left, p.Right, p.Pos))
	}
	ret := strings.Join(strs, ",")
	/* 	if len(ret) >= 50 {
	   		epos := strings.LastIndex(ret, ",")
	   		if epos != -1 {
	   			ret = ret[0:epos]
	   		}
	   	}
	*/return ret
}

func selectBookLines(bookID int) ([]db.Chars, error) {
	const stmt = "SELECT Cor,OCR,LineID FROM " + db.ContentsTableName +
		" WHERE BookID=? ORDER BY PageID, LineID, Seq"
	rows, err := db.Query(service.Pool(), stmt, bookID)
	if err != nil {
		return nil, fmt.Errorf("select lines for book ID %d: %v",
			bookID, err)
	}
	defer rows.Close()
	lineID := -1
	var lines []db.Chars
	for rows.Next() {
		var tmp int
		var char db.Char
		if err := rows.Scan(&char.Cor, &char.OCR, &tmp); err != nil {
			return nil, fmt.Errorf("select lines for book ID %d: %v",
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

func eachLine(ctx context.Context, bid int, f func(db.Chars) error) error {
	const stmt = "SELECT Cor,OCR,LineID FROM " + db.ContentsTableName +
		" WHERE BookID=? ORDER BY PageID, LineID, Seq"
	rows, err := db.QueryContext(ctx, service.Pool(), stmt, bid)
	if err != nil {
		return fmt.Errorf("load lines for book ID %d: %v", bid, err)
	}
	defer rows.Close()
	lineID := -1
	var line db.Chars
	for rows.Next() {
		var tmp int
		var char db.Char
		if err := rows.Scan(&char.Cor, &char.OCR, &tmp); err != nil {
			return fmt.Errorf("load line for book ID %d: %v", bid, err)
		}
		if tmp != lineID {
			if len(line) > 0 {
				if err := f(line); err != nil {
					return fmt.Errorf("load line for book ID %d: %v", bid, err)
				}
			}
			lineID = tmp
			line = line[0:0]
		}
		line = append(line, char)
	}
	if len(line) > 0 {
		if err := f(line); err != nil {
			return fmt.Errorf("load line for book ID %d: %v", bid, err)
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

func trim(chars db.Chars) db.Chars {
	i, j := 0, len(chars)
	for ; i < j; i++ {
		c := chars[i].Cor
		if c == 0 {
			c = chars[i].OCR
		}
		if !unicode.IsPunct(c) && !unicode.IsSymbol(c) {
			break
		}
	}
	for ; j > i; j-- {
		c := chars[j-1].Cor
		if c == 0 {
			c = chars[j-1].OCR
		}
		if !unicode.IsPunct(c) && !unicode.IsSymbol(c) {
			break
		}
	}
	return chars[i:j]
}

type logger struct{}

func (logger) Log(msg string) {
	if len(msg) > 0 {
		log.Printf(msg)
	}
}
