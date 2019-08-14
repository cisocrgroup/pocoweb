package main // import "github.com/finkf/pcwpostcorrection"

import (
	"context"
	"flag"
	"net/http"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	dsn     = "user:pass@proto(host)/dbname"
	listen  = ":80"
	baseDir = "/"
	debug   = false
	config  = ""
)

func init() {
	flag.StringVar(&listen, "listen", listen, "set host")
	flag.BoolVar(&debug, "debug", debug, "enable debugging")
	flag.StringVar(&dsn, "dsn", dsn, "set mysql connection DSN")
	flag.StringVar(&baseDir, "base", baseDir, "set project base dir")
	flag.StringVar(&config, "config", config, "set base config file for post correction")
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
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		if !d.Project.Status["profiled"] {
			service.ErrorResponse(w, http.StatusBadRequest,
				"project not profiled")
			return
		}
		f(w, r, d)
	}
}

func getExtendedLexicon() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		res := api.ExtendedLexicon{
			Yes: map[string]int{
				"dÿß":      1,
				"ordenung": 3,
				"dieczů":   10,
			},
			No: map[string]int{
				"Sũmer": 1,
				"welff": 3,
			},
		}
		service.JSONResponse(w, res)
	}
}

func runExtendedLexicon() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		jobID, err := jobs.Start(context.Background(), elRunner{project: d.Project})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func runDecisionMaker() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		jobID, err := jobs.Start(context.Background(), rrdmRunner{project: d.Project})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func getDecisionMaker() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		res := api.PostCorrection{
			Always:    map[string]int{"zeÿten": 3},
			Sometimes: map[string]int{"kallter": 8, "~peis": 9},
			Never:     map[string]int{"ordenung": 4},
		}
		service.JSONResponse(w, res)
	}
}