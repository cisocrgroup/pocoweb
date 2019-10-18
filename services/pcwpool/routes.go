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

	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
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
	s.router.HandleFunc("/global", service.WithLog(service.WithMethods(
		http.MethodGet, s.handleGetGlobalPool())))
	s.router.HandleFunc("/users/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithIDs(s.handleGetUserPool(), "users"))))
}

func (s *server) handleGetUserPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		const stmnt = `
SELECT b.author,b.title,b.description,u.email,b.bookid,b.year,b.pooled
FROM books b
JOIN projects p ON p.origin=b.bookid
JOIN users u on p.owner=u.id
WHERE p.origin=p.id and p.owner=?
`
		userid := ctx.Value("users").(int)
		rows, err := s.pool.Query(stmnt, userid)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot query pooled books for user id %d: %v", userid, err)
			return
		}
		defer rows.Close()
		if err := s.writePool(w, rows); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot write pool for user id %d: %v", userid, err)
			return
		}
	}
}

func (s *server) handleGetGlobalPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
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
		if err := s.writePool(w, rows); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot write pool: %v", err)
			return
		}
	}
}

func (s *server) writePool(w http.ResponseWriter, rows *sql.Rows) (err error) {
	w.Header().Add("Content-Type", "application/zip")
	zipw := zip.NewWriter(w)
	defer func() {
		e := zipw.Close()
		if err == nil {
			err = e
		}
	}()
	for rows.Next() {
		var book bookInfo
		if err := book.scan(rows); err != nil {
			return fmt.Errorf("cannot scan book: %v", err)
		}
		if err := book.write(zipw); err != nil {
			return fmt.Errorf("cannot write info for book %s: %v", book.String(), err)
		}
		if err := s.writeBookToPool(&book, zipw); err != nil {
			return fmt.Errorf("cannot pool book %s: %v", book.String(), err)
		}
	}
	return nil
}

func (s *server) writeBookToPool(book *bookInfo, out *zip.Writer) error {
	const stmnt = `
SELECT c.pageid,c.lineid,c.cor,c.ocr,l.imagepath
FROM contents c
JOIN textlines l ON c.bookid=l.bookid,c.pageid=l.pageid,c.lineid=l.lineid
WHERE c.bookid=?
ORDER BY c.pageid,c.lineid,c.seq
`
	rows, err := s.pool.Query(stmnt, book.ID)
	if err != nil {
		return fmt.Errorf("cannot query text content for book %s: %v", book.String(), err)
	}
	defer rows.Close()
	var line lineInfo
	for rows.Next() {
		var pid, lid, cor, ocr int
		if err := rows.Scan(&pid, &lid, &cor, &ocr); err != nil {
			return fmt.Errorf("cannot scan content for book %s: %v", book.String(), err)
		}
		// same line on same page -> append char
		if pid == line.pageID && lid == line.lineID {
			line.line = append(line.line, db.Char{Cor: rune(cor), OCR: rune(ocr)})
			continue
		}
		// write old line
		if err := line.write(s.base, book.String(), out); err != nil {
			return fmt.Errorf("cannot write line %d: %v", line.lineID, err)
		}
		// scan image path
		if err := rows.Scan(nil, nil, nil, nil, &line.path); err != nil {
			return fmt.Errorf("cannot scan content for book %s: %v", book.String(), err)
		}
		line.line = line.line[:0] // clear
		line.pageID = pid
		line.lineID = lid
		line.line = append(line.line, db.Char{Cor: rune(cor), OCR: rune(ocr)})
	}
	return nil
}

type lineInfo struct {
	line           db.Chars
	path           string
	pageID, lineID int
}

func (line *lineInfo) write(base, book string, out *zip.Writer) error {
	// only write fully corrected, not empty lines
	if len(line.line) == 0 || !line.line.IsFullyCorrected() {
		return nil
	}
	if err := line.writeGT(book, out); err != nil {
		return err
	}
	return line.copyImage(base, book, out)
}

func (line *lineInfo) writeGT(book string, out *zip.Writer) error {
	path := filepath.Join("corpus", book, fmt.Sprintf("%4d/%4d.gt.txt", line.pageID, line.lineID))
	w, err := out.Create(path)
	if err != nil {
		return fmt.Errorf("cannot create gt file for line %d: %v", line.lineID, err)
	}
	// Note: w is a writer not a WriteCloser
	if _, err := fmt.Fprintln(w, line.line.Cor()); err != nil {
		return fmt.Errorf("cannot create write gt for line %d: %v", line.lineID, err)
	}
	return nil
}

func (line *lineInfo) copyImage(base, book string, out *zip.Writer) error {
	path := filepath.Join("corpus", book, fmt.Sprintf("%4d/%4d.png", line.pageID, line.lineID))
	dest, err := out.Create(path)
	if err != nil {
		return fmt.Errorf("cannot create image file: %v", err)
	}
	// Note: dest is a writer not a WriteCloser
	src, err := os.Open(filepath.Join(base, line.path))
	if err != nil {
		return fmt.Errorf("cannot open source image file: %v", err)
	}
	defer src.Close()
	if _, err := io.Copy(dest, src); err != nil {
		return fmt.Errorf("cannot copy image file: %v", err)
	}
	return nil
}

type bookInfo struct {
	Author, Title, Description, OwnerEmail string
	ID, Year                               int
	Pooled                                 bool
}

func (book *bookInfo) scan(rows *sql.Rows) error {
	return rows.Scan(&book.Author, &book.Title, &book.Description, &book.OwnerEmail,
		&book.ID, &book.Year, &book.Pooled)
}

func (book *bookInfo) write(out *zip.Writer) error {
	w, err := out.Create(filepath.Join("corpus", book.String()+".json"))
	if err != nil {
		return fmt.Errorf("cannot create file info file: %v", err)
	}
	// Note: w is a writer not a WriteCloser
	if err := json.NewEncoder(w).Encode(book); err != nil {
		return fmt.Errorf("cannot encode file info file: %v", err)
	}
	return nil
}

func (book *bookInfo) String() string {
	return fmt.Sprintf("%4d-%s_%s",
		book.Year,
		strings.ToLower(strings.ReplaceAll(book.Author, " ", "_")),
		strings.ToLower(strings.ReplaceAll(book.Title, " ", "_")),
	)
}
