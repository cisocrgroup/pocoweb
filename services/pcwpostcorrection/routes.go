package main

import (
	"context"
	"database/sql"
	"encoding/json"
	"net/http"
	"os"
	"path/filepath"

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
			http.MethodPut, service.WithProject(withPutData(s.handlePutExtendedLexicon())),
			http.MethodPost, service.WithProject(withProfiledProject(s.handleRunExtendedLexicon())))))
	s.router.HandleFunc("/postcorrect/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(s.handleGetPostCorrection()),
			http.MethodPost, service.WithProject(withProfiledProject(s.handleRunPostCorrection())))))
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

func withPutData(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		var put api.ExtendedLexicon
		if err := json.NewDecoder(r.Body).Decode(&put); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest, "invalid data: %v", err)
			return
		}
		f(context.WithValue(ctx, "put", put), w, r)
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

func (s *server) handleGetExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		path := filepath.Join(s.baseDir, p.Directory, "postcorrection", "le-protocol.json")
		in, err := os.Open(path)
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
		var protocol leProtocol
		if err := json.NewDecoder(in).Decode(&protocol); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot decode protocol: %v", err)
			return
		}
		var ret api.ExtendedLexicon
		protocol.toAPI(&ret)
		ret.ProjectID = p.ProjectID
		ret.BookID = p.BookID
		service.JSONResponse(w, ret)
	}
}

func (s *server) handlePutExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		put := ctx.Value("put").(api.ExtendedLexicon)
		path := filepath.Join(s.baseDir, p.Directory, "postcorrection", "le-protocol.json")
		in, err := os.Open(path)
		if os.IsNotExist(err) {
			service.ErrorResponse(w, http.StatusNotFound, "cannot find protocol: %s", path)
			return
		}
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError, "cannot open protocol: %v", err)
			return
		}
		defer in.Close()
		var protocol leProtocol
		if err := json.NewDecoder(in).Decode(&protocol); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError, "cannot decode protocol: %v", err)
			return
		}
		out, err := os.Create(path)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError, "cannot write protocol: %v", err)
			return
		}
		// Handle defer manually
		protocol.updateFromAPI(&put)
		if err := json.NewEncoder(out).Encode(protocol); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError, "cannot read protocol: %v", err)
			out.Close() // ignore error
			return
		}
		if err := out.Close(); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot close protocol: %v", err)
		}
		// Reload updated protocol and rewrite it into the api.
		put.ProjectID = p.ProjectID
		put.BookID = p.BookID
		put.Yes = nil
		put.No = nil
		protocol.toAPI(&put)
		service.JSONResponse(w, put)
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

func writeEmptyPostCorrection(w http.ResponseWriter, p *db.Project) {
	ret := api.PostCorrection{
		ProjectID: p.ProjectID,
		BookID:    p.BookID,
		Always:    map[string]int{},
		Sometimes: map[string]int{},
		Never:     map[string]int{},
	}
	service.JSONResponse(w, ret)
}

func (s *server) handleRunPostCorrection() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		jobID, err := jobs.Start(context.Background(), pcRunner{pool: s.pool, project: p})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func (s *server) handleGetPostCorrection() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		protocol := filepath.Join(s.baseDir, p.Directory, "postcorrection", "dm-protocol-pcw.json")
		if _, err := os.Stat(protocol); os.IsNotExist(err) {
			writeEmptyPostCorrection(w, p)
			return
		}
		w.Header().Add("Content-Type", "application/json")
		http.ServeFile(w, r, protocol)
	}
}

type leProtocolValue struct {
	Count      int     `json:"count"`
	Confidence float64 `json:"confidence"`
}

type leProtocol struct {
	Yes map[string]leProtocolValue `json:"yes"`
	No  map[string]leProtocolValue `json:"no"`
}

func (p *leProtocol) toAPI(api *api.ExtendedLexicon) {
	api.Yes = make(map[string]int)
	api.No = make(map[string]int)
	for k, v := range p.Yes {
		api.Yes[k] = v.Count
	}
	for k, v := range p.No {
		api.No[k] = v.Count
	}
}

func (p *leProtocol) updateFromAPI(api *api.ExtendedLexicon) {
	if p.Yes == nil {
		p.Yes = make(map[string]leProtocolValue)
	}
	if p.No == nil {
		p.No = make(map[string]leProtocolValue)
	}
	for k := range api.Yes {
		if _, ok := p.No[k]; ok {
			p.Yes[k] = p.No[k]
			delete(p.No, k)
		}
	}
	for k := range api.No {
		if _, ok := p.Yes[k]; ok {
			p.No[k] = p.Yes[k]
			delete(p.Yes, k)
		}
	}
}
