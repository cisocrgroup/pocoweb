package main

import (
	"compress/gzip"
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"math/rand"
	"os"
	"path/filepath"
	"strings"
	"time"
	"unicode"

	"github.com/finkf/gofiler"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
)

type leRunner struct {
	baseRunner
}

func (r leRunner) Name() string {
	return "calculating extended lexicon"
}

func (r leRunner) Run(ctx context.Context) error {
	return r.simpleRun()
}

func (r leRunner) simpleRun() error {
	profile, err := readProfileDir(r.project.Directory)
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

func readProfileDir(dir string) (gofiler.Profile, error) {
	return readProfile(filepath.Join(baseDir, dir, "profile.json.gz"))
}

func readProfile(path string) (gofiler.Profile, error) {
	fin, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("cannot open profile %s: %v", path, err)
	}
	defer fin.Close()
	in, err := gzip.NewReader(fin)
	if err != nil {
		return nil, fmt.Errorf("cannot open profile %s: %v", path, err)
	}
	defer in.Close()
	var profile gofiler.Profile
	if err := json.NewDecoder(in).Decode(&profile); err != nil {
		return nil, fmt.Errorf("cannot decode profile %s: %v", path, err)
	}
	return profile, nil
}

func writeProtocol(dir, file string, protocol interface{}) (eout error) {
	path := filepath.Join(baseDir, dir, file)
	out, err := os.Create(path)
	if err != nil {
		return fmt.Errorf("cannot write protocol %s: %v", path, err)
	}
	defer func() {
		etmp := out.Close()
		if eout == nil {
			eout = etmp
		}
	}()
	if err := json.NewEncoder(out).Encode(protocol); err != nil {
		return fmt.Errorf("cannot encode protocol %s: %v", path, err)
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

type retrainer struct {
	project int
}

func (r retrainer) BookID() int {
	return r.project
}

func (r retrainer) Name() string {
	return "retrainer"
}

func (r retrainer) Run(ctx context.Context) error {
	duration := time.Duration(rand.Intn(30)+1) * time.Second
	select {
	case <-time.After(duration):
		return nil
	case <-ctx.Done():
		return nil
	}
}
