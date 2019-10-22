package main // import "github.com/finkf/pcwpostcorrection"

import (
	"context"
	"encoding/json"
	"flag"
	"net/http"
	"os"
	"path/filepath"
	"strings"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	dsn        = "user:pass@proto(host)/dbname"
	listen     = ":80"
	pocowebURL = "http://pocoweb"
	baseDir    = "/"
	debug      = false
	config     = ""
)

func init() {
	flag.StringVar(&listen, "listen", listen, "set host")
	flag.BoolVar(&debug, "debug", debug, "enable debugging")
	flag.StringVar(&dsn, "dsn", dsn, "set mysql connection DSN")
	flag.StringVar(&baseDir, "base", baseDir, "set project base dir")
	flag.StringVar(&config, "config", config, "set base config file for post correction")
	flag.StringVar(&config, "pocoweb", pocowebURL, "set pocoweb url")
}

func must(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func main() {
	flag.Parse()
	if debug {
		log.SetLevel(log.DebugLevel)
	}
	must(service.Init(dsn))
	defer service.Close()
	jobs.Init(service.Pool())
	defer jobs.Close()
	http.HandleFunc("/postcorrect/el/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getExtendedLexicon()),
			http.MethodPost, service.WithProject(withProfiledProject(runExtendedLexicon())))))

	http.HandleFunc("/postcorrect/rrdm/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getDecisionMaker()),
			http.MethodPost, service.WithProject(withProfiledProject(runDecisionMaker())))))

	log.Infof("listening on %s", listen)
	must(http.ListenAndServe(listen, nil))
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

func emptyExtendedLexicon(w http.ResponseWriter, p *db.Project) {
	ret := api.ExtendedLexicon{
		ProjectID: p.ProjectID,
		BookID:    p.BookID,
		Yes:       map[string]int{},
		No:        map[string]int{},
	}
	service.JSONResponse(w, ret)
}

func getExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		protocol := filepath.Join(baseDir, p.Directory, "postcorrection", "le-protocol.json")
		in, err := os.Open(protocol)
		if os.IsNotExist(err) { // just send an empty answer
			emptyExtendedLexicon(w, p)
			return
		}
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot open protocol: %v", err)
			return
		}
		defer in.Close()
		var ret api.ExtendedLexicon
		if err := json.NewDecoder(in).Decode(&ret); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot decode protocol: %v", err)
			return
		}
		ret.ProjectID = p.ProjectID
		ret.BookID = p.BookID
		service.JSONResponse(w, ret)
	}
}

func runExtendedLexicon() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		jobID, err := jobs.Start(context.Background(), elRunner{project: p})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func emptyDecisionMaker(w http.ResponseWriter, p *db.Project) {
	ret := api.PostCorrection{
		ProjectID: p.ProjectID,
		BookID:    p.BookID,
		Always:    map[string]int{},
		Sometimes: map[string]int{},
		Never:     map[string]int{},
	}
	service.JSONResponse(w, ret)
}

func runDecisionMaker() service.HandlerFunc {
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

func getDecisionMaker() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		protocol := filepath.Join(baseDir, p.Directory, "postcorrection", "dm-protocol.json")
		if _, err := os.Stat(protocol); os.IsNotExist(err) {
			emptyDecisionMaker(w, p)
			return
		}
		protocol = strings.ReplaceAll(protocol, ".json", "-pcw.json")
		w.Header().Add("Content-Type", "application/json")
		http.ServeFile(w, r, protocol)
	}
}
