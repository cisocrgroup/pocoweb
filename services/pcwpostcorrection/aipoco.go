package main

import (
	"context"
	"database/sql"
	"fmt"
	"path/filepath"
	"strconv"
	"unicode"
	"unicode/utf8"

	"git.sr.ht/~flobar/apoco/pkg/apoco"
	"git.sr.ht/~flobar/apoco/pkg/apoco/align"
	"git.sr.ht/~flobar/apoco/pkg/apoco/ml"
	"git.sr.ht/~flobar/gotess"
	"git.sr.ht/~flobar/lev"
	"github.com/UNO-SOFT/ulog"
	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
)

var _ jobs.Runner = &aipoco{}

type aipoco struct {
	pool                                   *sql.DB
	project                                *db.Project
	tessAPI                                gotess.API
	baseDir, modelDir, tessDir, pocowebURL string
}

func (ai *aipoco) BookID() int {
	return ai.project.BookID
}

func (ai *aipoco) Name() string {
	return "running post-correction"
}

func (ai *aipoco) Run(ctx context.Context) error {
	// Read the profile and remove all bogus entries from the profile.
	profilePath := filepath.Join(ai.baseDir, ai.project.Directory, "profile.json.gz")
	profile, err := readProfile(profilePath)
	if err != nil {
		return fmt.Errorf("aipoco run: %v", err)
	}
	for ocr, interp := range profile {
		if len(interp.Candidates) == 0 || interp.Candidates[0].Distance == 0 {
			delete(profile, ocr)
		}
	}
	// Set up tesseract api, open the model,
	// create the secondary OCR and then orrect the tokens.
	if err := ai.setupTess(ctx); err != nil {
		return fmt.Errorf("aipoco run: %v", err)
	}
	defer ai.tessAPI.Close()
	m, err := ai.openModels(2)
	if err != nil {
		return fmt.Errorf("aipoco run: %v", err)
	}
	err = apoco.Pipe(
		ctx,
		ai.lines(),
		ai.tokens(true),
		apoco.FilterBad(2),
		ai.beforeNormalization(),
		apoco.Normalize(),
		ai.afterNormalization(),
		apoco.FilterShort(4),
		apoco.ConnectLanguageModel(m.lms),
		apoco.ConnectUnigrams(),
		apoco.ConnectProfile(profile),
		apoco.ConnectCandidates(),
		apoco.ConnectRankings(m.lr.rr, m.fs.rr, 2),
		apoco.ConnectCorrections(m.lr.dm, m.fs.dm, 2),
		ai.correct(),
	)
	if err != nil {
		return fmt.Errorf("aipoco run: %v", err)
	}
	return nil
}

func (ai *aipoco) correct() apoco.StreamFunc {
	return func(ctx context.Context, in <-chan apoco.T, _ chan<- apoco.T) error {
		corrections := api.PostCorrection{
			BookID:      ai.project.BookID,
			ProjectID:   ai.project.ProjectID,
			Corrections: map[string]api.PostCorrectionToken{},
		}
		ulog.Write("correcting")
		err := apoco.EachToken(ctx, in, func(t apoco.T) error {
			correction := t.Payload.(apoco.Correction)
			// t.ID, t.Tokens[0], t.Tokens[1], correction.Candidate.Suggestion, correction.Conf > 0.5)
			var taken bool
			if correction.Conf > 0.5 {
				taken = true
			}
			var bid, pid, lid, tid int
			if _, err := fmt.Sscanf(t.ID, "%d:%d:%d:%d", &bid, &pid, &lid, &tid); err != nil {
				return fmt.Errorf("correct token: bad ID %s", t.ID)
			}
			corrections.Corrections[t.ID] = api.PostCorrectionToken{
				BookID:     ai.project.BookID,
				ProjectID:  ai.project.ProjectID,
				PageID:     pid,
				LineID:     lid,
				TokenID:    tid,
				Normalized: t.Tokens[0],
				OCR:        t.Tokens[len(t.Tokens)-1], // Unnormalized original primary OCR token.
				Cor:        correction.Candidate.Suggestion,
				Confidence: correction.Conf,
				Taken:      taken,
			}
			return nil
		})
		if err != nil {
			return fmt.Errorf("correct: %v", err)
		}
		ulog.Write("correcting in backend")
		if err := ai.correctInBackend(ctx, &corrections); err != nil {
			return fmt.Errorf("correct: %v", err)
		}
		ulog.Write("correcting in database")
		if err := ai.correctInDatabase(ctx, &corrections); err != nil {
			return fmt.Errorf("correct: %v", err)
		}
		ulog.Write("correcting done")
		return nil
	}
}

func (ai *aipoco) correctInBackend(ctx context.Context, corrections *api.PostCorrection) error {
	client := api.NewClient(ai.pocowebURL, true)
	for _, t := range corrections.Corrections {
		if !t.Taken {
			continue
		}
		cor := apoco.ApplyOCRToCorrection(t.OCR, t.Cor)
		ulog.Write("correcting token in backend", "normalized", t.Normalized, "OCR", t.OCR, "Cor", cor)
		url := client.URL("books/%d/pages/%d/lines/%d/tokens/%d?t=%s&len=%d",
			t.BookID, t.PageID, t.LineID, t.TokenID, "automatic", utf8.RuneCountInString(t.OCR))
		err := client.Put(url, struct {
			Cor string `json:"correction"`
		}{cor}, nil)
		if err != nil { // TODO: handle StatusConflict errors
			return fmt.Errorf("correct in backend: %v", err)
		}
	}
	return nil
}

// func ApplyOCRToCorrection(ocr, sug string) string {
func (ai *aipoco) correctInDatabase(ctx context.Context, corrections *api.PostCorrection) error {
	transaction := db.NewTransaction(ai.pool.Begin())
	transaction.Do(func(dtb db.DB) error {
		const del = "DELETE FROM autocorrections WHERE bookid = ?"
		if _, err := db.ExecContext(ctx, ai.pool, del, ai.project.BookID); err != nil {
			return fmt.Errorf("insert protocol: delete protocol: %v", err)
		}
		ins, err := ai.pool.Prepare("INSERT INTO autocorrections" +
			"(bookid,pageid,lineid,tokenid,ocrtypid,cortypid,conf,taken) " +
			"VALUES(?,?,?,?,?,?,?,?)")
		if err != nil {
			return fmt.Errorf("insert protocol: %v", err)
		}
		defer ins.Close()
		t, err := db.NewTypeInserter(ai.pool)
		if err != nil {
			return fmt.Errorf("insert protocol: %v", err)
		}
		defer t.Close()

		for _, v := range corrections.Corrections {
			ocrtypid, err := t.ID(v.Normalized)
			if err != nil {
				return fmt.Errorf("insert protocol: insert ocr type: %v", err)
			}
			cortypid, err := t.ID(v.Cor)
			if err != nil {
				return fmt.Errorf("insert protocol: insert cor type: %v", err)
			}
			if _, err := ins.ExecContext(ctx, v.BookID, v.PageID, v.LineID, v.TokenID, ocrtypid, cortypid, v.Confidence, v.Taken); err != nil {
				return fmt.Errorf("insert protocol: %v", err)
			}
		}
		// Update postcorrected field in books.
		const status = "UPDATE books SET postcorrected = true WHERE bookid = ?"
		if _, err := db.ExecContext(ctx, ai.pool, status, ai.project.BookID); err != nil {
			return fmt.Errorf("update status: %v", err)
		}
		return nil
	})
	return transaction.Done()
}

func (ai *aipoco) tokens(alev bool) apoco.StreamFunc {
	return func(ctx context.Context, in <-chan apoco.T, out chan<- apoco.T) error {
		var matrix lev.Mat
		mat := &matrix
		if !alev {
			mat = nil
		}
		return apoco.EachToken(ctx, in, func(line apoco.T) error {
			alignments := alignLines(mat, line.Tokens)
			var ts []apoco.T
			for i := range alignments {
				t := apoco.T{
					File:     line.File,
					Document: line.Document,
					ID:       line.ID + ":" + strconv.Itoa(i+1),
				}
				for j, p := range alignments[i] {
					// Handle leading punctuation (and symbols) to
					// use the correct token ID for the primary OCR token.
					if j == 0 {
						var k int
						slice := p.Slice()
						for k < len(slice) && (unicode.IsPunct(slice[k]) || unicode.IsSymbol(slice[k])) {
							k++
						}
						t.Chars = line.Chars[p.B:p.E]
						t.ID = line.ID + ":" + strconv.Itoa(p.B+k)
					}
					t.Tokens = append(t.Tokens, string(p.Slice()))
				}
				ts = append(ts, t)
			}
			if len(ts) > 0 { // Mark last token in the line.
				ts[len(ts)-1].EOL = true
			}
			if err := apoco.SendTokens(ctx, out, ts...); err != nil {
				return fmt.Errorf("token: %v", err)
			}
			return nil
		})
	}
}

func (ai *aipoco) beforeNormalization() apoco.StreamFunc {
	return func(ctx context.Context, in <-chan apoco.T, out chan<- apoco.T) error {
		return apoco.EachToken(ctx, in, func(t apoco.T) error {
			// Hack: put the primary unnormalized OCR
			// (t.Tokens[0]) into each token's payload to
			// keep the unnormalized ocr token.
			t.Payload = t.Tokens[0]
			return apoco.SendTokens(ctx, out, t)
		})

	}
}

func (ai *aipoco) afterNormalization() apoco.StreamFunc {
	return func(ctx context.Context, in <-chan apoco.T, out chan<- apoco.T) error {
		return apoco.EachToken(ctx, in, func(t apoco.T) error {
			// Hack: take the unnormalized OCR from
			// payload and append it to the end of
			// t.Tokens.
			t.Tokens = append(t.Tokens, t.Payload.(string))
			return apoco.SendTokens(ctx, out, t)
		})
	}
}

func alignLines(mat *lev.Mat, lines []string) [][]align.Pos {
	rs := make([][]rune, len(lines))
	for i := range lines {
		rs[i] = []rune(lines[i])
	}
	if mat != nil {
		return align.Lev(mat, rs[0], rs[1:]...)
	}
	return align.Do(rs[0], rs[1:]...)
}

func (ai *aipoco) lines() apoco.StreamFunc {
	return func(ctx context.Context, _ <-chan apoco.T, out chan<- apoco.T) error {
		var document apoco.Document
		return ai.eachLine(ctx, func(pid, lid int, line apoco.Chars) error {
			sec, err := ai.secondaryOCR(ctx, pid, lid)
			if err != nil {
				return err
			}
			t := apoco.T{
				Document: &document,
				Tokens:   []string{line.Chars(), sec},
				Chars:    line,
				ID:       fmt.Sprintf("%d:%d:%d", ai.project.BookID, pid, lid),
			}
			return apoco.SendTokens(ctx, out, t)
		})
	}
}

func (ai *aipoco) eachLine(ctx context.Context, f func(int, int, apoco.Chars) error) error {
	const stmt = `
SELECT PageID,LineID,OCR,Cor,Conf
FROM contents
WHERE BookID=?
ORDER BY PageID,LineID,Seq`
	rows, err := db.QueryContext(ctx, ai.pool, stmt, ai.project.BookID)
	if err != nil {
		return fmt.Errorf("each line: select lines for book ID %d: %v", ai.project.BookID, err)
	}
	defer rows.Close()
	lineID, pageID := -1, -1
	var line db.Chars
	for rows.Next() {
		var pid, lid int
		var c db.Char
		if err := rows.Scan(&pid, &lid, &c.OCR, &c.Cor, &c.Conf); err != nil {
			return fmt.Errorf("each line: select lines for book ID %d: %v", ai.project.BookID, err)
		}
		if lid == lineID && pid == pageID {
			line = append(line, c)
			continue
		}
		if len(line) > 0 {
			if err := f(pageID, lineID, db2apoco(line)); err != nil {
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

func db2apoco(chars db.Chars) apoco.Chars {
	ret := make(apoco.Chars, 0, len(chars))
	for _, c := range chars {
		if c.Cor == -1 {
			continue
		}
		if c.Cor != 0 {
			ret = append(ret, apoco.Char{
				Char: c.Cor,
				Conf: c.Conf,
			})
			continue
		}
		ret = append(ret, apoco.Char{
			Char: c.OCR,
			Conf: c.Conf,
		})
	}
	return ret
}

func (ai *aipoco) setupTess(ctx context.Context) error {
	const stmt = `SELECT lang FROM books WHERE bookid=?`
	rows, err := db.QueryContext(ctx, ai.pool, stmt, ai.project.BookID)
	if err != nil {
		return fmt.Errorf("setup tess: %v", err)
	}
	defer rows.Close()
	if !rows.Next() {
		return fmt.Errorf("setup tess: no result")
	}
	var lang string
	if err := rows.Scan(&lang); err != nil {
		return fmt.Errorf("setup tess: %v", err)
	}
	lang = mapLang(lang)
	ai.tessAPI, err = gotess.New(ai.tessDir, lang)
	if err != nil {
		return fmt.Errorf("setup tess: %v", err)
	}
	return nil
}

func (ai *aipoco) secondaryOCR(ctx context.Context, pid, lid int) (string, error) {
	const stmt = `
SELECT imagepath
FROM textlines
WHERE bookid=? AND pageid=? AND lineid=?`
	fail := func(err error) (string, error) {
		return "", fmt.Errorf("secondary OCR: %v", err)
	}

	rows, err := db.QueryContext(ctx, ai.pool, stmt, ai.project.BookID, pid, lid)
	if err != nil {
		return fail(fmt.Errorf("select lines for book ID %d: %v", ai.project.BookID, err))
	}
	defer rows.Close()
	if !rows.Next() {
		return fail(fmt.Errorf("select lines for book ID %d: no result", ai.project.BookID))
	}
	var imagepath string
	if err := rows.Scan(&imagepath); err != nil {
		return fail(fmt.Errorf("select lines for book ID %d: %v", ai.project.BookID, err))
	}
	imagepath = filepath.Join(ai.baseDir, imagepath)
	if err := ai.tessAPI.SetImagePNG(imagepath); err != nil {
		return fail(fmt.Errorf("set image %s: %v", imagepath, err))
	}
	if err := ai.tessAPI.Recognize(); err != nil {
		return fail(fmt.Errorf("recognize %s: %v", imagepath, err))
	}
	return ai.tessAPI.Line(), nil
}

func mapLang(lang string) string {
	m := map[string]string{
		"german":      "deu",
		"german-frak": "deu-frak",
	}
	if ret, ok := m[lang]; ok {
		return ret
	}
	return lang
}

type lr struct {
	rr, dm *ml.LR
}

type fs struct {
	rr, dm apoco.FeatureSet
}

type modelData struct {
	lr  lr
	fs  fs
	lms map[string]*apoco.FreqList
}

func (ai *aipoco) openModels(nOCR int) (*modelData, error) {
	path := filepath.Join(ai.modelDir, "19th.bin")
	fail := func(err error) (*modelData, error) {
		return nil, fmt.Errorf("open model %s: %v", path, err)
	}

	m, err := apoco.ReadModel(path, nil, false)
	if err != nil {
		return fail(err)
	}
	rr, rrFS, err := m.Get("rr", nOCR)
	if err != nil {
		return fail(err)
	}
	dm, dmFS, err := m.Get("rr", nOCR)
	if err != nil {
		return fail(err)
	}
	return &modelData{
		lr:  lr{rr, dm},
		fs:  fs{rrFS, dmFS},
		lms: m.LM,
	}, nil
}
