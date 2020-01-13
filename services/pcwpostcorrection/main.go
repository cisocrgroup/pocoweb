package main // import "github.com/finkf/pcwpostcorrection"

import (
	"flag"
	"net/http"

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
	flag.StringVar(&pocowebURL, "pocoweb", pocowebURL, "set pocoweb url")
}

func must(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func main() {
	flag.Parse()
	must(service.InitDebug(dsn, debug))
	defer service.Close()
	jobs.Init(service.Pool())
	defer jobs.Close()
	s := server{
		pool:       service.Pool(),
		baseDir:    baseDir,
		pocowebURL: pocowebURL,
	}
	s.routes()
	log.Infof("listening on %s", listen)
	must(http.ListenAndServe(listen, &s))
}
