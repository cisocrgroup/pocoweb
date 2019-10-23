package main

import (
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strings"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
)

type server struct {
	baseDir    string
	pocowebURL string
	router     *http.ServeMux
	pool       *sql.DB
}

func (s *server) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	s.router.ServeHTTP(w, r)
}

func (s *server) routes() {
	s.router = http.DefaultServeMux
	s.router.HandleFunc("/postcorrect/le/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(s.handleGetExtendedLexicon()),
			http.MethodPost, service.WithProject(withProfiledProject(s.handleRunExtendedLexicon())))))
	s.router.HandleFunc("/postcorrect/rrdm/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(s.handleGetDecisionMaker()),
			http.MethodPost, service.WithProject(withProfiledProject(s.handleRunDecisionMaker())))))
}

func withProfiledProject(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		if !p.Status["profiled"] {
			service.ErrorResponse(w, http.StatusBadRequest, "project not profiled")
			return
		}
		f(ctx, w, r)
	}
}

func writeEmptyExtendedLexicon(w http.ResponseWriter, p *db.Project) {
	ret := api.ExtendedLexicon{
		ProjectID: p.ProjectID,
		BookID:    p.BookID,
		Yes:       map[string]int{},
		No:        map[string]int{},
	}
	service.JSONResponse(w, ret)
}

func readLEProtcol(el *api.ExtendedLexicon, in io.Reader) error {
	var protocol struct {
		Yes map[string]struct{ Count int }
		No  map[string]struct{ Count int }
	}
	if err := json.NewDecoder(in).Decode(&protocol); err != nil {
		return fmt.Errorf("cannot decode protocol: %v", err)
	}
	el.Yes = make(map[string]int)
	el.No = make(map[string]int)
	for k, v := range protocol.Yes {
		el.Yes[k] = v.Count
	}
	for k, v := range protocol.No {
		el.No[k] = v.Count
	}
	return nil
}

func (s *server) handleGetExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		protocol := filepath.Join(s.baseDir, p.Directory, "postcorrection", "le-protocol.json")
		in, err := os.Open(protocol)
		if os.IsNotExist(err) { // just send an empty answer
			writeEmptyExtendedLexicon(w, p)
			return
		}
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot open protocol: %v", err)
			return
		}
		defer in.Close()
		var ret api.ExtendedLexicon
		if err := readLEProtcol(&ret, in); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot read protocol: %v", err)
			return
		}
		ret.ProjectID = p.ProjectID
		ret.BookID = p.BookID
		service.JSONResponse(w, ret)
	}
}

func (s *server) handleRunExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		jobID, err := jobs.Start(context.Background(), leRunner{project: p})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func writeEmptyDecisionMaker(w http.ResponseWriter, p *db.Project) {
	ret := api.PostCorrection{
		ProjectID: p.ProjectID,
		BookID:    p.BookID,
		Always:    map[string]int{},
		Sometimes: map[string]int{},
		Never:     map[string]int{},
	}
	service.JSONResponse(w, ret)
}

func (s *server) handleRunDecisionMaker() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		jobID, err := jobs.Start(context.Background(), rrdmRunner{project: p})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func (s *server) handleGetDecisionMaker() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		protocol := filepath.Join(s.baseDir, p.Directory, "postcorrection", "dm-protocol.json")
		if _, err := os.Stat(protocol); os.IsNotExist(err) {
			writeEmptyDecisionMaker(w, p)
			return
		}
		protocol = strings.ReplaceAll(protocol, ".json", "-pcw.json")
		w.Header().Add("Content-Type", "application/json")
		http.ServeFile(w, r, protocol)
	}
}
