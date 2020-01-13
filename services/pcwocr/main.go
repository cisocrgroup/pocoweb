package main // import "github.com/finkf/pcwocr"

import (
	"context"
	"encoding/json"
	"flag"
	"io/ioutil"
	"math/rand"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	debug      bool
	listen     string
	modelDir   string
	baseDir    string
	dsn        string
	configfile string
	valfak     float64
	models     []Model
	config     configuration
)

func init() {
	flag.StringVar(&modelDir, "models", "", "set directory for models")
	flag.StringVar(&baseDir, "base", "", "set base directory")
	flag.StringVar(&listen, "listen", ":80", "set listen host:port")
	flag.StringVar(&dsn, "dsn", "", "set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.StringVar(&configfile, "config", "/etc/pcwocr.toml", "set configuration file path")
	flag.Float64Var(&valfak, "valfak", 0.1, "set size of validation set")
	flag.BoolVar(&debug, "debug", false, "enable debugging output")
	rand.Seed(time.Now().UnixNano())
}

func must(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func main() {
	flag.Parse()
	must(loadConfig(configfile))
	if debug {
		log.SetLevel(log.DebugLevel)
	}
	must(service.Init(dsn))
	defer service.Close()
	jobs.Init(service.Pool())
	defer jobs.Close()
	models = loadModels(modelDir)
	http.Handle("/ocr/", service.WithLog(service.WithMethods(
		http.MethodGet, getGlobalModels())))
	http.Handle("/ocr/predict/books/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithProject(getModels()),
		http.MethodPost, service.WithProject(
			service.WithIDs(withModel(predict()), "books", "?pages", "?lines")))))
	http.Handle("/ocr/train/books/", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithProject(withModel(train())))))
	log.Infof("listening on %s", listen)
	must(http.ListenAndServe(listen, nil))
}

func getGlobalModels() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		var gModels api.Models
		for _, model := range models {
			gModels.Models = append(gModels.Models, model.Model)
		}
		service.JSONResponse(w, gModels)
	}
}

func getModels() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		var gModels api.Models
		for _, model := range loadProjectModels(&d.Project.Book) {
			gModels.Models = append(gModels.Models, model.Model)
		}
		service.JSONResponse(w, gModels)
	}
}

func predict() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		jobID, err := jobs.Start(context.Background(), predictor{d})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func train() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		jobID, err := jobs.Start(context.Background(), trainer{d})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run job: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func withModel(f service.HandlerFunc) service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		var model api.Model
		if err := json.NewDecoder(r.Body).Decode(&model); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot decode model: %v", err)
			return
		}
		m, ok := findModel(loadProjectModels(&d.Project.Book), model)
		if !ok {
			service.ErrorResponse(w, http.StatusNotFound,
				"cannot find model: %s", model.Name)
			return
		}
		d.Post = m
		f(w, r, d)
	}
}

func loadProjectModels(b *db.Book) []Model {
	return append(loadModels(projectModelDir(b)), models...)
}

func projectModelDir(p *db.Book) string {
	return filepath.Join(modelDir, "project-models", filepath.Base(p.Directory))
}

func findModel(models []Model, search api.Model) (Model, bool) {
	for _, model := range models {
		log.Debugf("checking %s with %s", search.Name, model.Name)
		if search.Name == model.Name {
			return model, true
		}
	}
	return Model{}, false
}

func loadModels(dir string) []Model {
	var models []Model
	err := filepath.Walk(dir, func(p string, fi os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !fi.IsDir() {
			return nil
		}
		if p == modelDir {
			return nil
		}
		models = append(models, Model{
			path: p,
			Model: api.Model{
				Name:        filepath.Base(p),
				Description: loadDescription(p),
			},
		})
		return nil
	})
	if err != nil {
		log.Printf("error loading models: %v", err)
	}
	return models
}

func loadDescription(p string) string {
	fp := p + ".desc.txt"
	is, err := os.Open(fp)
	if err != nil {
		log.Errorf("cannot load description: %v", err)
		return strings.Replace(filepath.Base(p), "_", " ", -1)
	}
	defer is.Close()
	desc, err := ioutil.ReadAll(is)
	if err != nil {
		log.Errorf("cannot load description: %v", err)
		return strings.Replace(filepath.Base(p), "_", " ", -1)
	}
	return strings.TrimSpace(string(desc))
}
