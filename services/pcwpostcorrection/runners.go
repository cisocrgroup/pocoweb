package main

import (
	"compress/gzip"
	"context"
	"database/sql"
	"encoding/json"
	"errors"
	"fmt"
	"math/rand"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"
	"unicode"

	"github.com/finkf/gofiler"
	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type leRunner struct {
	baseRunner
}

func (r leRunner) Name() string {
	return "calculating extended lexicon"
}

func (r leRunner) Run(ctx context.Context) error {
	return r.simpleRun()
	// if err := r.setupWorkspace(); err != nil {
	// 	return fmt.Errorf(
	// 		"cannot calculate extended lexicon: %v", err)
	// }
	// if err := r.runEL(ctx); err != nil {
	// 	return fmt.Errorf(
	// 		"cannot calculate extended lexicon: %v", err)
	// }
	// return nil
}

func (r leRunner) setupWorkspace() error {
	doc, err := loadDocument(r.project)
	if err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	if err := doc.write(); err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	return nil
}

func (r leRunner) runEL(ctx context.Context) error {
	const script = "/apps/run.bash"
	err := jobs.Run(
		ctx,
		script,
		"le",
		filepath.Join(baseDir, r.project.Directory, "postcorrection"),
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
	)
	if err != nil {
		return fmt.Errorf("cannot run %s: %v", script, err)
	}
	const stmnt = "UPDATE " + db.BooksTableName + " SET extendedlexicon=? WHERE bookid=?"
	_, err = db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot update database: %v", err)
	}
	return nil
}

func (r leRunner) simpleRun() error {
	profile, err := readProfile(r.project.Directory)
	if err != nil {
		return fmt.Errorf("cannot create lexicon extension: %v", err)
	}
	protocol := leProtocol{
		Yes: make(map[string]leProtocolValue),
		No:  make(map[string]leProtocolValue),
	}
	r.eachLine(func(_, _ int, line db.Chars) error {
		return eachTrimmedWord(line, func(word db.Chars) error {
			ocr := strings.ToLower(word.Cor())
			if _, ok := profile[ocr]; !ok {
				return nil
			}
			interp := profile[ocr]
			if len(interp.Candidates) == 0 {
				return nil
			}
			topCand := interp.Candidates[0]
			if topCand.Distance == 0 && len(topCand.HistPatterns) == 0 {
				return nil
			}
			if topCand.Distance > 0 {
				return nil
			}
			weight := float64(topCand.Weight) * word.AverageConfidence()
			if weight > .90 {
				delete(protocol.No, ocr)
				protocol.Yes[ocr] = leProtocolValue{
					Count:      interp.N,
					Confidence: weight,
				}
			} else if _, ok := protocol.Yes[ocr]; !ok {
				protocol.No[ocr] = leProtocolValue{
					Count:      interp.N,
					Confidence: weight,
				}
			}
			return nil
		})
	})
	if err := writeProtocol(r.project.Directory, "le_protocol.json", protocol); err != nil {
		return fmt.Errorf("cannot create lexicon extension: %v", err)
	}
	const stmnt = "UPDATE books SET extendedlexicon=? WHERE bookid=?"
	_, err = db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot update database: %v", err)
	}
	return nil
}

type pcRunner struct {
	baseRunner
}

func (r pcRunner) Name() string {
	return "calculating post-correction"
}

func (r pcRunner) Run(ctx context.Context) error {
	return r.simpleRun()
	// if err := r.setupWorkspace(); err != nil {
	// 	return fmt.Errorf(
	// 		"cannot calculate post correction: %v", err)
	// }
	// dir := filepath.Join(baseDir, r.project.Directory, "postcorrection")
	// protocol := filepath.Join(dir, "dm-protocol.json")
	// err := jobs.Run(
	// 	ctx,
	// 	"/apps/run.bash",
	// 	"dm",
	// 	dir,
	// 	filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
	// )
	// if err != nil {
	// 	return fmt.Errorf("cannot run /apps/run.bash: %v", err)
	// }
	// if err := r.correct(protocol); err != nil {
	// 	return fmt.Errorf("cannot correct: %v", err)
	// }
	// return nil
}

func (r pcRunner) setupWorkspace() error {
	doc, err := loadDocument(r.project)
	if err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	if err := doc.write(); err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	return nil
}

func (r pcRunner) correct(path string) (err error) {
	var corrections api.PostCorrection
	if err := r.readProtocol(&corrections, path); err != nil {
		return fmt.Errorf("cannot read protocol %s: %v", path, err)
	}
	rnd := rand.New(rand.NewSource(time.Now().Unix()))
	// preprocess protocol
	for k, v := range corrections.Corrections {
		var bid, pid, lid, tid int
		if _, err := fmt.Sscanf(k, "%d:%d:%d:%d", &bid, &pid, &lid, &tid); err != nil {
			return fmt.Errorf("invalid protocol id: %s", k)
		}
		v.BookID = r.project.BookID
		v.ProjectID = r.project.ProjectID
		v.PageID = pid
		v.LineID = lid
		v.TokenID = tid
		if rnd.Int31n(10) < 3 { // 0, 1, 2
			v.Taken = true
		}
		corrections.Corrections[k] = v
	}

	if err := r.correctInBackend(&corrections); err != nil {
		return fmt.Errorf("cannot correct in backend: %v", err)
	}
	if err := r.correctInDatabase(&corrections); err != nil {
		return fmt.Errorf("cannot correct in database: %v", err)
	}
	if err := r.updateStatus(); err != nil {
		return fmt.Errorf("cannot update project status: %v", err)
	}
	if err := r.writeProtocol(&corrections, path); err != nil {
		return fmt.Errorf("cannot write protocol: %v", err)
	}
	return nil
}

func (r pcRunner) updateStatus() error {
	const stmnt = "UPDATE " + db.BooksTableName + " SET postcorrected=? WHERE bookid=?"
	_, err := db.Exec(r.pool, stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot execute database update: %v", err)
	}
	return nil
}

func (r pcRunner) deleteCorrections() error {
	const del = "DELETE FROM autocorrections WHERE bookid = ?"
	_, err := db.Exec(r.pool, del, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot delete old corrections from database: %v", err)
	}
	return nil
}

func (r pcRunner) readProtocol(pc *api.PostCorrection, path string) error {
	in, err := os.Open(path)
	if err != nil {
		return fmt.Errorf("cannot open protocol: %v", err)
	}
	defer in.Close()
	if err := json.NewDecoder(in).Decode(pc); err != nil {
		return fmt.Errorf("cannot decode protocol: %v", err)
	}
	return nil
}

func (r pcRunner) writeProtocol(pc *api.PostCorrection, path string) (err error) {
	// path = strings.Replace(path, ".json", "-pcw.json", 1)
	out, err := os.Create(path)
	if err != nil {
		return fmt.Errorf("cannot open post correction: %v", err)
	}
	defer func() {
		e := out.Close()
		if err == nil {
			err = e
		}
	}()
	if err := json.NewEncoder(out).Encode(pc); err != nil {
		return fmt.Errorf("cannot encode post correction: %v", err)
	}
	return nil
}

func (r pcRunner) correctInBackend(corrections *api.PostCorrection) error {
	// We should be communicating within docker compose - so skip verify is ok.
	client := api.NewClient(pocowebURL, true)
	for k, v := range corrections.Corrections {
		log.Debugf("correction: %s %v", k, v)
		if !v.Taken { // only correct corrections that should be taken
			continue
		}
		token := api.Token{
			BookID:    v.BookID,
			ProjectID: v.ProjectID,
			PageID:    v.PageID,
			LineID:    v.LineID,
			TokenID:   v.TokenID,
		}
		err := client.PutTokenX(&token, api.CorAutomatic, v.Cor)
		if err != nil {
			var ex api.ErrInvalidResponseCode
			if !errors.As(err, &ex) {
				return fmt.Errorf("cannot post correct %s: %v", v.Normalized, err)
			}
			if ex.Code != http.StatusConflict {
				return fmt.Errorf("cannot post correct %s: %v", v.Normalized, err)
			}
			log.Debugf("cannot autocorrect: already manually corrected: %s", k)
			v.Taken = false
			corrections.Corrections[k] = v
		}
	}
	return nil
}

func (r pcRunner) correctInDatabase(corrections *api.PostCorrection) error {
	if err := r.deleteCorrections(); err != nil {
		return err
	}
	ins, err := r.pool.Prepare("INSERT INTO autocorrections" +
		"(bookid,pageid,lineid,tokenid,ocrtypid,cortypid,taken) " +
		"VALUES(?,?,?,?,?,?,?)")
	if err != nil {
		return fmt.Errorf("cannot prepare insert statement: %v", err)
	}
	defer ins.Close()
	t, err := db.NewTypeInserter(r.pool)
	if err != nil {
		return fmt.Errorf("cannot insert types: %v", err)
	}
	defer t.Close()

	// insert corrections
	for _, v := range corrections.Corrections {
		ocrtypid, err := t.ID(v.Normalized)
		if err != nil {
			return fmt.Errorf("cannot insert ocr type: %v", err)
		}
		cortypid, err := t.ID(v.Cor)
		if err != nil {
			return fmt.Errorf("cannot insert cor type: %v", err)
		}
		if _, err := ins.Exec(v.BookID, v.PageID, v.LineID, v.TokenID,
			ocrtypid, cortypid, v.Taken); err != nil {
			return fmt.Errorf("cannot insert correction: %v", err)
		}
	}
	return nil
}

func (r pcRunner) simpleRun() error {
	profile, err := readProfile(r.project.Directory)
	if err != nil {
		return fmt.Errorf("cannot post correct: %v", err)
	}
	for ocr, interp := range profile {
		if len(interp.Candidates) == 0 || interp.Candidates[0].Distance == 0 {
			delete(profile, ocr)
		}
	}
	corrections := api.PostCorrection{
		BookID:      r.project.BookID,
		ProjectID:   r.project.ProjectID,
		Corrections: make(map[string]api.PostCorrectionToken),
	}
	err = r.eachLine(func(pid, lid int, line db.Chars) error {
		return eachTrimmedWord(line, func(word db.Chars) error {
			str := strings.ToLower(word.Cor())
			if str == "" {
				return nil
			}
			if _, ok := profile[str]; !ok {
				return nil
			}
			id := fmt.Sprintf("%d:%d:%d:%d", r.project.BookID, pid, lid, word.ID())
			topCand := profile[str].Candidates[0]
			take := topCand.Weight > .98
			log.Debugf("correcting %s %s/%s", id, word.Cor(), str)
			corrections.Corrections[id] = api.PostCorrectionToken{
				BookID:     r.project.BookID,
				ProjectID:  r.project.ProjectID,
				PageID:     pid,
				LineID:     lid,
				TokenID:    word.ID(),
				Normalized: str,
				OCR:        word.Cor(), // this is OK!
				Cor:        applyCasing(word.Cor(), topCand.Suggestion),
				Confidence: float64(topCand.Weight),
				Taken:      take,
			}
			return nil
		})
	})
	if err != nil {
		return fmt.Errorf("cannot post correct: %v", err)
	}
	if err := r.correctInBackend(&corrections); err != nil {
		return fmt.Errorf("cannot correct in backend: %v", err)
	}
	if err := r.correctInDatabase(&corrections); err != nil {
		return fmt.Errorf("cannot correct in database: %v", err)
	}
	if err := r.updateStatus(); err != nil {
		return fmt.Errorf("cannot update project status: %v", err)
	}
	path := filepath.Join(baseDir, r.project.Directory, "dm_protocol.json")
	if err := r.writeProtocol(&corrections, path); err != nil {
		return fmt.Errorf("cannot write protocol: %v", err)
	}
	return nil
}

type baseRunner struct {
	pool    *sql.DB
	project *db.Project
}

func (r baseRunner) BookID() int {
	return r.project.BookID
}

func (r baseRunner) eachLine(f func(int, int, db.Chars) error) error {
	const stmt = "SELECT PageID,LineID,OCR,Cor,Cut,Conf,Seq,CID,Manually FROM " +
		db.ContentsTableName +
		" WHERE BookID=? ORDER BY PageID, LineID, Seq"
	rows, err := db.Query(r.pool, stmt, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot select lines for book ID %d: %v",
			r.project.BookID, err)
	}
	defer rows.Close()
	lineID, pageID := -1, -1
	var line db.Chars
	for rows.Next() {
		var pid, lid int
		var c db.Char
		if err := rows.Scan(&pid, &lid, &c.OCR, &c.Cor, &c.Cut, &c.Conf, &c.Seq, &c.ID, &c.Manually); err != nil {
			return fmt.Errorf("cannot select lines for book ID %d: %v",
				r.project.BookID, err)
		}
		if lid == lineID && pid == pageID {
			line = append(line, c)
			continue
		}
		if len(line) > 0 {
			if err := f(pageID, lineID, line); err != nil {
				return err
			}
		}
		line = line[0:0]
		line = append(line, c)
		lineID = lid
		pageID = pid
	}
	return nil
}

func eachTrimmedWord(line db.Chars, f func(db.Chars) error) error {
	for t, r := line.NextWord(); len(t) > 0; t, r = r.NextWord() {
		if err := f(trim(t)); err != nil {
			return err
		}
	}
	return nil
}

func readProfile(dir string) (gofiler.Profile, error) {
	path := filepath.Join(baseDir, dir, "profile.json.gz")
	fin, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("cannot open profile: %v", err)
	}
	defer fin.Close()
	in, err := gzip.NewReader(fin)
	if err != nil {
		return nil, fmt.Errorf("cannot open profile: %v", err)
	}
	defer in.Close()
	var profile gofiler.Profile
	if err := json.NewDecoder(in).Decode(&profile); err != nil {
		return nil, fmt.Errorf("cannot decode profile: %v", err)
	}
	return profile, nil
}

func writeProtocol(dir, file string, protocol interface{}) (eout error) {
	path := filepath.Join(baseDir, dir, file)
	out, err := os.Create(path)
	if err != nil {
		return fmt.Errorf("cannot write protocol: %v", err)
	}
	defer func() {
		etmp := out.Close()
		if eout == nil {
			eout = etmp
		}
	}()
	if err := json.NewEncoder(out).Encode(protocol); err != nil {
		return fmt.Errorf("cannot encode protocol: %v", err)
	}
	return nil
}

func trim(chars db.Chars) db.Chars {
	return chars.Trim(func(c db.Char) bool {
		x := c.Cor
		if x == 0 {
			x = c.OCR
		}
		return unicode.IsPunct(x)
	})
}

func applyCasing(template, str string) string {
	wtmpl := []rune(template)
	wstr := []rune(str)
	for i := 0; i < len(wtmpl) && i < len(wstr); i++ {
		if unicode.IsUpper(wtmpl[i]) {
			wstr[i] = unicode.ToUpper(wstr[i])
		} else if unicode.IsLower(wtmpl[i]) {
			wstr[i] = unicode.ToLower(wstr[i])
		}
	}
	return string(wstr)
}
