package main

import (
	"encoding/xml"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"io"
	"os"
	"path/filepath"
	"strconv"

	"github.com/finkf/gocrd/xml/page"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type pagex struct {
	db.Page
	lines []db.Line
}

// return /project-data/unique-str-id/postcorrection/src/imgs/pageid.image-ext
func (p *pagex) destImgPath(doc *document) string {
	return filepath.Join(
		doc.imgs,
		fmt.Sprintf("%04d%s", p.PageID, filepath.Ext(p.ImagePath)),
	)
}

func (p *pagex) srcImgPath() string {
	return filepath.Join(baseDir, p.ImagePath)
}

// return /project-data/unique-str-id/postcorrection/src/xmls/pageid.xml
func (p *pagex) destXMLPath(doc *document) string {
	return filepath.Join(
		doc.xmls,
		fmt.Sprintf("%04d.xml", p.PageID),
	)
}

type document struct {
	project          *db.Project
	pages            map[int]*pagex
	base, imgs, xmls string
}

func loadDocument(p *db.Project) (*document, error) {
	base := filepath.Join(baseDir, p.Directory, "postcorrection")
	imgs := filepath.Join(base, "src", "imgs")
	xmls := filepath.Join(base, "src", "xmls")
	doc := document{project: p, base: base, imgs: imgs, xmls: xmls}

	if err := doc.loadPages(); err != nil {
		return nil, fmt.Errorf("cannot load document: %v", err)
	}
	if err := doc.loadLines(); err != nil {
		return nil, fmt.Errorf("cannot load document: %v", err)
	}
	if err := doc.loadContents(); err != nil {
		return nil, fmt.Errorf("cannot load document: %v", err)
	}
	return &doc, nil
}

func (doc *document) loadPages() error {
	const stmnt = "SELECT bookid,pageid,imagepath,pleft,ptop,pright,pbottom " +
		"FROM " + db.PagesTableName + " " +
		"WHERE bookid=?"
	rows, err := db.Query(service.Pool(), stmnt, doc.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot load pages: %v", err)
	}
	defer rows.Close()
	doc.pages = make(map[int]*pagex)
	for rows.Next() {
		var tmp db.Page
		if err := rows.Scan(&tmp.BookID, &tmp.PageID, &tmp.ImagePath,
			&tmp.Left, &tmp.Top, &tmp.Right, &tmp.Bottom); err != nil {
			return fmt.Errorf("cannot load pages: %v", err)
		}
		doc.pages[tmp.PageID] = &pagex{Page: tmp}
	}
	return nil
}

func (doc *document) loadLines() error {
	const stmnt = "SELECT bookid,pageid,lineid,imagepath,lleft,ltop,lright,lbottom " +
		"FROM " + db.TextLinesTableName + " " +
		"WHERE bookid=?"
	rows, err := db.Query(service.Pool(), stmnt, doc.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot load lines: %v", err)
	}
	defer rows.Close()
	for rows.Next() {
		var tmp db.Line
		if err := rows.Scan(&tmp.BookID, &tmp.PageID, &tmp.LineID, &tmp.ImagePath,
			&tmp.Left, &tmp.Top, &tmp.Right, &tmp.Bottom); err != nil {
			return fmt.Errorf("cannot load lines: %v", err)
		}
		doc.pages[tmp.PageID].lines = append(doc.pages[tmp.PageID].lines, tmp)
	}
	return nil
}

func (doc *document) loadContents() error {
	const stmnt = "SELECT pageid,lineid,ocr,cor,cut,conf,seq " +
		"FROM " + db.ContentsTableName + " " +
		"WHERE bookid=? ORDER BY pageid,lineid,seq"
	rows, err := db.Query(service.Pool(), stmnt, doc.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot load contents: %v", err)
	}
	defer rows.Close()
	for rows.Next() {
		var pid, lid int
		var c db.Char
		if err := rows.Scan(&pid, &lid, &c.OCR, &c.Cor, &c.Cut, &c.Conf, &c.Seq); err != nil {
			return fmt.Errorf("cannot load contents: %v", err)
		}
		if lid == 0 || doc.pages[pid].lines[lid-1].LineID != lid {
			return fmt.Errorf("cannot load contents: invalid line id: %d", lid)
		}
		doc.pages[pid].lines[lid-1].Chars = append(doc.pages[pid].lines[lid-1].Chars, c)
	}
	return nil
}

func (doc document) write() error {
	if err := os.MkdirAll(doc.imgs, 0755); err != nil {
		return fmt.Errorf("cannot write pages: %v", err)
	}
	if err := os.MkdirAll(doc.xmls, 0755); err != nil {
		return fmt.Errorf("cannot write pages: %v", err)
	}
	for id, pagex := range doc.pages {
		if len(pagex.lines) == 0 || pagex.lines[0].ImagePath == "" {
			continue
		}
		if err := doc.writePage(pagex); err != nil {
			return fmt.Errorf("cannot write page %d: %v", id, err)
		}
	}
	return nil
}

func (doc document) writePage(p *pagex) error {
	if p.ImagePath == "" {
		if err := doc.createPageImage(p); err != nil {
			return fmt.Errorf("cannot create page image: %v", err)
		}
	}
	if err := doc.hardLinkOrCopyImage(p); err != nil {
		return fmt.Errorf("cannot copy image file: %v", err)
	}
	return doc.writePageXML(p)
}

func (doc document) createPageImage(p *pagex) error {
	if err := doc.writeImageFromLineSnippets(p); err != nil {
		return err
	}
	if err := doc.updateImageDataInDatabase(p); err != nil {
		return err
	}
	return nil
}

func (doc document) calculateRectangle(p *pagex) image.Rectangle {
	var r image.Rectangle
	for _, l := range p.lines {
		r.Max.Y += l.Bottom
		if r.Max.X < l.Right {
			r.Max.X = l.Right
		}
	}
	return r
}

func (doc document) writeImageFromLineSnippets(p *pagex) (err error) {
	imgpath := filepath.Join(
		filepath.Dir(p.lines[0].ImagePath),
		fmt.Sprintf("%04d.png", p.PageID))
	r := doc.calculateRectangle(p)
	pageImg := image.NewRGBA(r)
	draw.Draw(pageImg, pageImg.Bounds(), &image.Uniform{color.White}, image.ZP, draw.Src)
	y := 0
	for i := range p.lines {
		limgpath := filepath.Join(baseDir, p.lines[i].ImagePath)
		in, err := os.Open(limgpath)
		if err != nil {
			return err
		}
		defer in.Close()
		lineImg, err := png.Decode(in)
		if err != nil {
			return err
		}
		dest := image.Rectangle{
			Min: image.Point{lineImg.Bounds().Min.X, y},
			Max: image.Point{lineImg.Bounds().Max.X, y + lineImg.Bounds().Max.Y},
		}
		draw.Draw(pageImg, dest, lineImg, image.ZP, draw.Src)
		y += lineImg.Bounds().Max.Y
		// update rectangle for the line
		p.lines[i].Left = dest.Min.X
		p.lines[i].Top = dest.Min.Y
		p.lines[i].Right = dest.Max.X
		p.lines[i].Bottom = dest.Max.Y
	}
	// update image data
	p.Left = r.Min.X
	p.Right = r.Max.X
	p.Top = r.Min.Y
	p.Bottom = r.Max.Y
	log.Debugf("writing image file: %s", filepath.Join(baseDir, imgpath))
	out, err := os.Create(filepath.Join(baseDir, imgpath))
	if err != nil {
		return err
	}
	defer func() {
		ex := out.Close()
		if err == nil {
			err = ex
		}
	}()
	if err := png.Encode(out, pageImg); err != nil {
		return err
	}
	p.ImagePath = imgpath
	return nil
}

func (doc document) updateImageDataInDatabase(p *pagex) error {
	t := db.NewTransaction(service.Pool().Begin())
	t.Do(func(dtb db.DB) error {
		const upage = "UPDATE " + db.PagesTableName + " " +
			"SET imagepath=?,pleft=?,pright=?,ptop=?,pbottom=? " +
			"WHERE bookid=? AND pageid=?"
		_, err := db.Exec(dtb, upage, p.ImagePath,
			p.Left, p.Right, p.Top, p.Bottom, p.BookID, p.PageID)
		if err != nil {
			return err
		}
		const uline = "UPDATE " + db.TextLinesTableName + " " +
			"SET lleft=?,lright=?,ltop=?,lbottom=? " +
			"WHERE bookid=? AND pageid=? AND lineid=?"
		for _, l := range p.lines {
			_, err := db.Exec(dtb, uline, l.Left, l.Right,
				l.Top, l.Bottom, l.BookID, l.PageID, l.LineID)
			if err != nil {
				return err
			}
		}
		return nil
	})
	return t.Done()
}

func (doc document) hardLinkOrCopyImage(p *pagex) (err error) {
	src := p.srcImgPath()
	dest := p.destImgPath(&doc)
	log.Debugf("linking %s to %s", src, dest)
	if err = os.Link(src, dest); err == nil || os.IsExist(err) {
		return nil
	}
	log.Debugf("cannot link: %v", err)
	in, err := os.Open(src)
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
		if err == nil {
			err = ex
		}
	}()
	if _, err := io.Copy(out, in); err != nil {
		return fmt.Errorf("cannot copy file: %v", err)
	}
	return nil
}

func (doc document) writePageXML(p *pagex) (err error) {
	pagexml := page.PcGts{
		Attributes: page.PcGtsXMLHeader,
		Metadata:   make(page.Metadata),
		Page: page.Page{
			ImageFilename: filepath.Join(baseDir, p.ImagePath),
			ImageHeight:   p.Bottom,
			ImageWidth:    p.Right,
			PrintSpace: page.PrintSpace{
				Coords: page.Coords{
					Points: []image.Point{
						image.Pt(p.Left, p.Top),
						image.Pt(p.Right, p.Bottom),
					},
				},
			},
		},
	}
	doc.appendMetadata(&pagexml, p)
	main := page.TextRegion{
		TextRegionBase: page.TextRegionBase{
			ID:     fmt.Sprintf("%d:%d", doc.project.BookID, p.PageID),
			Coords: pagexml.Page.PrintSpace.Coords,
		},
		Type: "page",
	}
	for _, l := range p.lines {
		doc.appendTextLine(&main, l)
	}
	pagexml.Page.TextRegion = append(pagexml.Page.TextRegion, main)
	outpath := p.destXMLPath(&doc)
	out, err := os.Create(outpath)
	if err != nil {
		return fmt.Errorf("cannot write page xml: %v", err)
	}
	defer func() {
		ex := out.Close()
		if err == nil {
			err = ex
		}
	}()
	// prepend default header
	if _, err := fmt.Fprint(out, xml.Header); err != nil {
		return fmt.Errorf("cannot write page xml: %v", err)
	}
	// There is a bug with image.Point,
	// so we need to encode using a pointer.
	if err := xml.NewEncoder(out).Encode(&pagexml); err != nil {
		return fmt.Errorf("cannot write page xml: %v", err)
	}
	return nil
}

func (doc document) appendTextLine(p *page.TextRegion, line db.Line) {
	xmlline := page.TextLine{
		TextRegionBase: page.TextRegionBase{
			ID: fmt.Sprintf("%d:%d:%d", doc.project.BookID, line.PageID, line.LineID),
			Coords: page.Coords{
				Points: []image.Point{
					image.Pt(line.Left, line.Top),
					image.Pt(line.Right, line.Bottom),
				},
			},
			AlternativeImage: &page.AlternativeImage{
				Filename: filepath.Join(baseDir, line.ImagePath),
				Comments: "alternative image line path",
			},
			Custom: fmt.Sprintf("manually-corrected:%t,automatically-corrected:%t,ocr:%s",
				line.Chars.IsManuallyCorrected(),
				line.Chars.IsAutomaticallyCorrected(),
				line.Chars.OCR(),
			),
			TextEquiv: page.TextEquiv{
				Unicode: []string{line.Chars.Cor()},
				Conf:    line.Chars.AverageConfidence(),
			},
		},
	}
	line.Chars.EachWord(func(word db.Chars, id int) {
		doc.appendWord(&xmlline, line, word, id)
	})
	if len(p.TextEquiv.Unicode) == 0 {
		p.TextEquiv.Unicode = append(p.TextEquiv.Unicode, line.Chars.Cor()+"\n")
	} else {
		p.TextEquiv.Unicode[0] += line.Chars.Cor() + "\n"
	}
	p.TextLine = append(p.TextLine, xmlline)
}

func (doc document) appendWord(l *page.TextLine, line db.Line, w db.Chars, id int) {
	left := line.Left
	if len(l.Word) > 0 {
		left = l.Word[len(l.Word)-1].Coords.Points[1].X + 1
	}
	xmlword := page.Word{
		TextRegionBase: page.TextRegionBase{
			ID: fmt.Sprintf("%s:%d", l.ID, id),
			Coords: page.Coords{
				Points: []image.Point{
					image.Pt(left, line.Top),
					image.Pt(w[len(w)-1].Cut, line.Bottom),
				},
			},
			Custom: fmt.Sprintf("manually-corrected:%t,automatically-corrected:%t,ocr:%s",
				w.IsManuallyCorrected(),
				w.IsAutomaticallyCorrected(),
				w.OCR(),
			),
			TextEquiv: page.TextEquiv{
				Unicode: []string{w.Cor()},
				Conf:    w.AverageConfidence(),
			},
		},
	}
	for _, c := range w {
		doc.appendGlyph(&xmlword, line, c)
	}
	l.Word = append(l.Word, xmlword)
}

func (doc document) appendGlyph(w *page.Word, line db.Line, c db.Char) {
	id := c.Seq
	left := line.Left
	if len(w.Glyph) > 0 {
		left = w.Glyph[len(w.Glyph)-1].Coords.Points[1].X + 1
	}
	char := c.OCR
	if c.Cor != 0 {
		char = c.Cor
	}
	if c.Cor == -1 { // skip deletions
		return
	}
	xmlglyph := page.Glyph{
		TextRegionBase: page.TextRegionBase{
			ID: fmt.Sprintf("%s:%d", w.ID, id),
			Coords: page.Coords{
				Points: []image.Point{
					image.Pt(left, line.Top),
					image.Pt(c.Cut, line.Bottom),
				},
			},
			TextEquiv: page.TextEquiv{
				Unicode: []string{string(char)},
				Conf:    c.Conf,
			},
		},
	}
	w.Glyph = append(w.Glyph, xmlglyph)
}

func (doc document) appendMetadata(pcgts *page.PcGts, p *pagex) {
	pcgts.Metadata["Creator"] = "pcwpostcorrection"
	pcgts.Metadata["ProjectDir"] = filepath.Join(baseDir, doc.project.Directory)
	pcgts.Metadata["BookID"] = strconv.Itoa(p.BookID)
	pcgts.Metadata["PageID"] = strconv.Itoa(p.PageID)
	pcgts.Metadata["Author"] = doc.project.Author
	pcgts.Metadata["Title"] = doc.project.Title
	pcgts.Metadata["Description"] = doc.project.Description
	pcgts.Metadata["Year"] = strconv.Itoa(doc.project.Year)
	pcgts.Metadata["Pages"] = strconv.Itoa(len(doc.pages))
}
