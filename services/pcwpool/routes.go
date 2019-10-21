package main

import (
	"archive/zip"
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type server struct {
	base   string
	router *http.ServeMux
	pool   *sql.DB
}

func (s *server) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	s.router.ServeHTTP(w, r)
}

func (s *server) routes() {
	s.router = http.DefaultServeMux
	s.router.HandleFunc("/pool/global", service.WithLog(service.WithMethods(
		http.MethodGet, s.handleGetGlobalPool())))
	s.router.HandleFunc("/pool/my", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(s.handleGetUserPool()))))
}

func (s *server) handleGetUserPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		const stmnt = `
SELECT b.author,b.title,b.description,u.email,b.lang,b.bookid,b.year,b.pooled
FROM books b
JOIN projects p ON p.origin=b.bookid
JOIN users u on p.owner=u.id
WHERE p.origin=p.id and p.owner=?
`
		userid := ctx.Value("auth").(*api.Session).User.ID
		rows, err := s.pool.Query(stmnt, userid)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot query pooled books for user id %d: %v", userid, err)
			return
		}
		defer rows.Close()
		w.Header().Add("Content-Type", "application/zip")
		// if _, err := io.Copy(w, &buf); err != nil {
		// 	return fmt.Errorf("cannot send archive: %v", err)
		// }
		if err := s.writePool(w, rows); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot write pool for user id %d: %v", userid, err)
			return
		}
	}
}

func (s *server) handleGetGlobalPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWnnriter, r *http.Request) {
		const stmnt = `
SELECT b.author,b.title,b.description,u.email,b.bookid,b.year,b.pooled
FROM books b
JOIN projects p ON p.origin=b.bookid
JOIN users u on p.owner=u.id
WHERE b.pooled=true and p.origin=p.id
`
		rows, err := s.pool.Query(stmnt)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot query pooled books: %v", err)
			return
		}
		defer rows.Close()
		w.Header().Add("Content-Type", "application/zip")
		if err := s.writePool(w, rows); err != nil {
			log.Infof("cannot write pool: %v", err)
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot write pool: %v", err)
			return
		}
	}
}

func (s *server) writePool(out io.Writer, rows *sql.Rows) (err error) {
	w := zip.NewWriter(out)
	defer func() {
		e := w.Close()
		if err == nil {
			err = e
		}
	}()
	for rows.Next() {
		var book bookInfo
		if err := book.scan(rows); err != nil {
			return fmt.Errorf("cannot scan book: %v", err)
		}
		if err := s.writeBookToPool(w, &book); err != nil {
			return fmt.Errorf("cannot pool book %s: %v", book.String(), err)
		}
		if book.NLines == 0 { // skip books with no corrected lines
			continue
		}
		if err := book.write(w); err != nil {
			return fmt.Errorf("cannot write info for book %s: %v", book.String(), err)
		}
	}
	return nil
}

func (s *server) writeBookToPool(out *zip.Writer, book *bookInfo) error {
	const stmnt = `
SELECT c.pageid,c.lineid,c.cor,c.ocr,l.imagepath
FROM contents c
JOIN textlines l ON c.bookid=l.bookid AND c.pageid=l.pageid AND c.lineid=l.lineid
WHERE c.bookid=?
ORDER BY c.pageid,c.lineid,c.seq
`
	rows, err := s.pool.Query(stmnt, book.ID)
	if err != nil {
		return fmt.Errorf("cannot query text content for book %s: %v",
			book.String(), err)
	}
	defer rows.Close()
	line := lineInfo{base: s.base, book: book}
	for rows.Next() {
		var pid, lid, cor, ocr int
		var path string
		if err := rows.Scan(&pid, &lid, &cor, &ocr, &path); err != nil {
			return fmt.Errorf("cannot scan content for book %s: %v",
				book.String(), err)
		}
		// same line on same page -> append char
		if pid == line.pageID && lid == line.lineID {
			line.line = append(line.line, db.Char{Cor: rune(cor), OCR: rune(ocr)})
			continue
		}
		// write old line
		if err := line.write(out); err != nil {
			return fmt.Errorf("cannot write line %d: %v", line.lineID, err)
		}
		line.line = line.line[:0] // clear
		line.path = path
		line.pageID = pid
		line.lineID = lid
		line.line = append(line.line, db.Char{Cor: rune(cor), OCR: rune(ocr)})
	}
	// last line
	if err := line.write(out); err != nil {
		return fmt.Errorf("cannot write line %d: %v", line.lineID, err)
	}
	return nil
}

type lineInfo struct {
	line           db.Chars
	path, base     string
	book           *bookInfo
	pageID, lineID int
}

func (line *lineInfo) write(out *zip.Writer) error {
	// only write fully corrected, not empty lines
	if len(line.line) == 0 || !line.line.IsFullyCorrected() {
		return nil
	}
	line.book.NLines++
	if err := line.writeGT(out); err != nil {
		return err
	}
	return line.copyImage(out)
}

func (line *lineInfo) writeGT(out *zip.Writer) error {
	w, err := out.CreateHeader(line.gtZIPHeader())
	if err != nil {
		return fmt.Errorf("cannot create gt file for line %d: %v",
			line.lineID, err)
	}
	// Note: w is a writer not a WriteCloser
	if _, err := fmt.Fprintln(w, line.line.Cor()); err != nil {
		return fmt.Errorf("cannot create write gt for line %d: %v",
			line.lineID, err)
	}
	return nil
}

func (line *lineInfo) copyImage(out *zip.Writer) error {
	dest, err := out.CreateHeader(line.pngZIPHeader())
	if err != nil {
		return fmt.Errorf("cannot create image file: %v", err)
	}
	// Note: dest is a writer not a WriteCloser
	src, err := os.Open(filepath.Join(line.base, line.path))
	if err != nil {
		return fmt.Errorf("cannot open source image file: %v", err)
	}
	defer src.Close()
	if _, err := io.Copy(dest, src); err != nil {
		return fmt.Errorf("cannot copy image file: %v", err)
	}
	return nil
}

func (line *lineInfo) gtZIPHeader() *zip.FileHeader {
	return &zip.FileHeader{
		Method:   zip.Deflate,
		Modified: time.Now(),
		Name: filepath.Join("corpus", line.book.String(),
			fmt.Sprintf("%04d/%04d.gt.txt", line.pageID, line.lineID)),
	}
}

func (line *lineInfo) pngZIPHeader() *zip.FileHeader {
	return &zip.FileHeader{
		Method:   zip.Deflate,
		Modified: time.Now(),
		Name: filepath.Join("corpus", line.book.String(),
			fmt.Sprintf("%04d/%04d.png", line.pageID, line.lineID)),
	}
}

type bookInfo struct {
	Author, Title, Description, Language, OwnerEmail string
	ID, Year, NLines                                 int
	Pooled                                           bool
}

func (book *bookInfo) scan(rows *sql.Rows) error {
	return rows.Scan(&book.Author, &book.Title, &book.Description,
		&book.OwnerEmail, &book.Language,
		&book.ID, &book.Year, &book.Pooled)
}

func (book *bookInfo) write(out *zip.Writer) error {
	w, err := out.CreateHeader(book.zipHeader())
	if err != nil {
		return fmt.Errorf("cannot create file info file: %v", err)
	}
	// Note: w is a writer not a WriteCloser
	if err := json.NewEncoder(w).Encode(book); err != nil {
		return fmt.Errorf("cannot encode file info file: %v", err)
	}
	return nil
}

func (book *bookInfo) zipHeader() *zip.FileHeader {
	return &zip.FileHeader{
		Method:   zip.Deflate,
		Modified: time.Now(),
		Name:     filepath.Join("corpus", book.String()+".json"),
	}
}

func (book *bookInfo) String() string {
	return fmt.Sprintf("%04d-%s_%s",
		book.Year,
		strings.ToLower(strings.ReplaceAll(book.Author, " ", "_")),
		strings.ToLower(strings.ReplaceAll(book.Title, " ", "_")),
	)
}
