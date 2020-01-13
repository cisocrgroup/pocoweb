package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"math/rand"
	"os"
	"path/filepath"
	"strings"
	"unicode/utf8"

	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type predictor struct {
	data *service.Data
}

func (p predictor) Name() string {
	return "predicting"
}

func (p predictor) BookID() int {
	return p.data.Project.BookID
}

func (p predictor) Run(ctx context.Context) error {
	ws, err := newWorkspace(p.data)
	if err != nil {
		return fmt.Errorf("cannot run prediction: %v", err)
	}
	if err := ws.predict(ctx); err != nil {
		return fmt.Errorf("cannot run prediction: %v", err)
	}
	return nil
}

type trainer struct {
	data *service.Data
}

func (t trainer) Name() string {
	return "training"
}

func (t trainer) BookID() int {
	return t.data.Project.BookID
}

func (t trainer) Run(ctx context.Context) error {
	ws, err := newWorkspace(t.data)
	if err != nil {
		return fmt.Errorf("cannot run training: %v", err)
	}
	if err := ws.train(ctx); err != nil {
		return fmt.Errorf("cannot run training: %v", err)
	}
	return nil
}

type id struct {
	bookID, pageID, lineID int
}

type workspace struct {
	book       *db.BookWithContent
	imgset     []string
	codec      map[rune]bool
	ids        map[string]id
	model      Model
	projectDir string
}

func newWorkspace(d *service.Data) (*workspace, error) {
	book, found, err := db.LoadBookWithContent(service.Pool(),
		d.Project.BookID, d.IDs["pages"], d.IDs["lines"])
	if err != nil {
		return nil, fmt.Errorf("cannot create workspace: %v", err)
	}
	if !found {
		return nil, fmt.Errorf("cannot create workspace: cannot find book id: %d",
			d.Project.BookID)
	}
	return &workspace{
		book:       book,
		model:      d.Post.(Model),
		ids:        make(map[string]id),
		codec:      make(map[rune]bool),
		projectDir: filepath.Join(baseDir, book.Directory, "ocr"),
	}, nil
}

func (ws *workspace) pageDir(pid int) string {
	return filepath.Join(ws.projectDir, fmt.Sprintf("%04d", pid))
}

func (ws *workspace) addPage(pid int) error {
	dir := ws.pageDir(pid)
	if err := os.MkdirAll(dir, 0755); err != nil && !os.IsExist(err) {
		return fmt.Errorf("cannot add page: %v", err)
	}
	return nil
}

func (ws *workspace) addLine(line db.Line, gt bool) error {
	if line.ImagePath == "" {
		return nil
	}
	src := filepath.Join(baseDir, line.ImagePath)
	dest := filepath.Join(ws.pageDir(line.PageID), filepath.Base(line.ImagePath))

	if gt {
		ws.addToCodec(line.Chars)
		gtdest := removeExtension(dest) + ".gt.txt"
		bs := []byte(line.Chars.Cor())
		if err := ioutil.WriteFile(gtdest, bs, 0644); err != nil {
			return fmt.Errorf("cannot add line: %v", err)
		}
	}
	if err := service.LinkOrCopy(src, dest); err != nil {
		return fmt.Errorf("cannot add line: %v", err)
	}
	ws.ids[dest] = id{line.BookID, line.PageID, line.LineID}
	ws.imgset = append(ws.imgset, dest)
	return nil
}

func (ws *workspace) codecString() string {
	codec := make([]rune, len(ws.codec))
	var i int
	for r := range ws.codec {
		codec[i] = r
		i++
	}
	return string(codec)
}

func (ws *workspace) addToCodec(cs db.Chars) {
	for _, c := range cs {
		if c.OCR != 0 && c.OCR != -1 {
			ws.codec[c.OCR] = true
		}
		if c.Cor != 0 && c.Cor != -1 {
			ws.codec[c.OCR] = true
		}
	}
}

func (ws *workspace) train(ctx context.Context) error {
	for _, page := range ws.book.Pages {
		if err := ws.addPage(page.PageID); err != nil {
			return fmt.Errorf("cannot train: %v", err)
		}
		for _, line := range page.Lines {
			if !line.Chars.IsFullyCorrected() {
				continue
			}
			if err := ws.addLine(line, true); err != nil {
				return fmt.Errorf("cannot train: %v", err)
			}
		}
	}
	limit := int(float64(len(ws.imgset)) * valfak)
	if limit < 1 || limit >= len(ws.imgset) {
		return fmt.Errorf("cannot train: not enough gt or train files")
	}
	rand.Shuffle(len(ws.imgset), func(i, j int) {
		ws.imgset[i], ws.imgset[j] = ws.imgset[j], ws.imgset[i]
	})
	dest, err := ws.copyModelDir()
	if err != nil {
		return fmt.Errorf("cannot train: %v", err)
	}
	if err := jobs.Run(ctx, "calamari-resume-training", ws.trainArgs(dest)...); err != nil {
		return fmt.Errorf("cannot train: %v", err)
	}
	return nil
}

func (ws *workspace) copyModelDir() (string, error) {
	name := fmt.Sprintf("%s_%s_%s",
		ws.model.Name,
		strings.Replace(ws.book.Author, " ", "_", -1),
		strings.Replace(ws.book.Title, " ", "_", -1),
	)
	dest := filepath.Join(projectModelDir(&ws.book.Book), name)
	if err := os.MkdirAll(dest, 0755); err != nil && err != os.ErrExist {
		return "", fmt.Errorf("cannot train: %v", err)
	}
	src := ws.model.path
	if src == dest {
		return dest, nil
	}
	return dest, filepath.Walk(src, func(p string, fi os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !fi.Mode().IsRegular() {
			return nil
		}
		if strings.HasSuffix(p, ".json") {
			return ws.fixConfigFilePaths(p, dest)
		}
		return copyFile(p, fi, dest)
	})
}

func (ws *workspace) fixConfigFilePaths(src, dest string) (err error) {
	config := make(map[string]interface{})
	in, err := os.Open(src)
	if err != nil {
		return fmt.Errorf("cannot fix config file: %v", err)
	}
	defer in.Close()
	if err := json.NewDecoder(in).Decode(&config); err != nil {
		return fmt.Errorf("cannot fix config file: %v", err)
	}
	config["earlyStoppingBestModelOutputDir"] = dest
	config["outputDir"] = dest
	out, err := os.Create(filepath.Join(dest, filepath.Base(src)))
	if err != nil {
		return fmt.Errorf("cannot fix config file: %v", err)
	}
	defer func() {
		ex := out.Close()
		if err != nil {
			err = ex
		}
	}()
	if err := json.NewEncoder(out).Encode(config); err != nil {
		return fmt.Errorf("cannot fix config file: %v", err)
	}
	return nil
}

func copyFile(p string, fi os.FileInfo, dir string) (err error) {
	dest := filepath.Join(dir, fi.Name())
	in, err := os.Open(p)
	if err != nil {
		return fmt.Errorf("cannot copy file: %v", err)
	}
	defer in.Close()
	out, err := os.Create(dest)
	if err != nil {
		return fmt.Errorf("cannot copy file: %v", err)
	}
	defer func() {
		ex := out.Close()
		if err != nil {
			err = ex
		}
	}()
	if _, err := io.Copy(out, in); err != nil {
		return fmt.Errorf("cannot copy file: %v", err)
	}
	return nil
}

func (ws *workspace) predict(ctx context.Context) error {
	// add setup workspace
	for _, page := range ws.book.Pages {
		if err := ws.addPage(page.PageID); err != nil {
			return fmt.Errorf("cannot predict: %v", err)
		}
		for _, line := range page.Lines {
			if line.Chars.IsPartiallyCorrected() { // implies fully corrected
				continue
			}
			if err := ws.addLine(line, false); err != nil {
				return fmt.Errorf("cannot predict: %v", err)
			}
		}
	}
	log.Debugf("running ocr with models %s", ws.model.Name)
	args := ws.predictArgs()
	if err := jobs.Run(ctx, "calamari-predict", args...); err != nil {
		return fmt.Errorf("cannot predict: %v", err)
	}
	return ws.updatePredictionsInDatabase()
}

func (ws *workspace) updatePredictionsInDatabase() error {
	for _, img := range ws.imgset {
		ids, ok := ws.ids[img]
		if !ok {
			log.Errorf("cannot update %s: cannot find ids", img)
			continue
		}
		data, err := readExtendedDataFromJSON(removeExtension(img) + ".json")
		if err != nil {
			log.Errorf("cannot update %s: %v", img, err)
			continue
		}
		// if we fail to update we fail
		if err := updateLineInDatabase(data, ids); err != nil {
			return fmt.Errorf("cannot update %s: %v", img, err)
		}
	}
	return nil
}

func updateLineInDatabase(data *ExtendedData, ids id) error {
	if len(data.Predictions) == 0 { // nothing to do
		return nil
	}
	t := db.NewTransaction(service.Pool().Begin())
	t.Do(func(dtb db.DB) error {
		const del = "DELETE FROM " + db.ContentsTableName + " " +
			"WHERE bookid=? AND pageid=? AND lineid=?"
		_, err := db.Exec(dtb, del, ids.bookID, ids.pageID, ids.lineID)
		if err != nil {
			return fmt.Errorf("cannot update line: %v", err)
		}
		const ins = "INSERT INTO " + db.ContentsTableName + " " +
			"(bookid,pageid,lineid,seq,ocr,cor,cut,conf) " +
			"VALUES (?,?,?,?,?,?,?,?)"
		for i, pos := range data.Predictions[0].Positions {
			if len(pos.Chars) == 0 {
				continue
			}
			if len(pos.Chars[0].Char) == 0 {
				continue
			}
			ocr := firstChar(pos.Chars[0].Char)
			if ocr == 0 {
				continue
			}
			_, err := db.Exec(dtb, ins, ids.bookID, ids.pageID, ids.lineID,
				i, ocr, 0, pos.GlobalEnd, pos.Chars[0].Probability)
			if err != nil {
				return fmt.Errorf("cannot update line: %v", err)
			}
		}
		return nil
	})
	return t.Done()
}

func (ws *workspace) predictArgs() []string {
	args := append(config.PredictArgs, "--extended_prediction_data")
	args = append(args, "--checkpoint")
	args = append(args, ws.model.path+"/*.ckpt")
	args = append(args, "--files")
	args = append(args, ws.imgset...)
	return args
}

func (ws *workspace) trainArgs(dest string) []string {
	limit := int(float64(len(ws.imgset)) * valfak)
	if limit <= 0 || limit >= len(ws.imgset) {
		panic("invalid limit")
	}
	args := append(config.TrainArgs, "--checkpoint")
	args = append(args, filepath.Join(dest, "0.ckpt"))
	args = append(args, "--validation")
	args = append(args, ws.imgset[0:limit]...)
	args = append(args, "--files")
	args = append(args, ws.imgset[limit:]...)
	return args
}

func firstChar(str string) int {
	r, _ := utf8.DecodeRuneInString(str)
	if r == utf8.RuneError {
		return 0
	}
	return int(r)
}

func removeExtension(p string) string {
	for ext := filepath.Ext(p); ext != ""; ext = filepath.Ext(p) {
		p = p[0 : len(p)-len(ext)]
	}
	return p
}
