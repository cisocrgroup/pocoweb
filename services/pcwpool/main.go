package main // import "github.com/cisocrgroup/pocoweb/services/pcwpool"

import (
	"database/sql"
	"flag"
	"net/http"

	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	dsn    string
	listen string
	debug  bool
)

func init() {
	flag.StringVar(&listen, "listen", ":8080", "set listening host")
	flag.StringVar(&dsn, "dsn", "", "set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.BoolVar(&debug, "debug", false, "enable debug logging")
}

func main() {
	flag.Parse()
	if err := service.InitDebug(dsn, debug); err != nil {
		log.Fatalf("cannot initialize service: %v", err)
	}
	defer service.Close()
	s := server{pool: service.Pool().(*sql.DB)}
	s.routes()
	log.Infof("starting to listen on %s", listen)
	if err := http.ListenAndServe(listen, &s); err != nil {
		log.Fatalf("cannot serve: %v", err)
	}
}
