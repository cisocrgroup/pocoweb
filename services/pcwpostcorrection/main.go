package main // import "github.com/finkf/pcwpostcorrection"

import (
	"flag"
	"net/http"

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
	modelDir   = "/models"
	tessDir    = "/tess"
	debug      = false
)

func init() {
	flag.StringVar(&listen, "listen", listen, "set host")
	flag.BoolVar(&debug, "debug", debug, "enable debugging")
	flag.StringVar(&dsn, "dsn", dsn, "set mysql connection DSN")
	flag.StringVar(&baseDir, "base", baseDir, "set project base dir")
	flag.StringVar(&modelDir, "model", baseDir, "set model base dir")
	flag.StringVar(&tessDir, "tess", baseDir, "set tesseract base dir")
	flag.StringVar(&pocowebURL, "pocoweb", pocowebURL, "set pocoweb url")
}

func chk(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func main() {
	flag.Parse()
	chk(service.InitDebug(dsn, debug))
	defer service.Close()
	jobs.Init(service.Pool())
	defer jobs.Close()
	s := server{
		pool:       service.Pool(),
		baseDir:    baseDir,
		pocowebURL: pocowebURL,
	}
	_, err := db.Exec(s.pool, "DELETE FROM jobs")
	chk(err)
	s.routes()
	log.Infof("listening on %s", listen)
	chk(http.ListenAndServe(listen, &s))
}
