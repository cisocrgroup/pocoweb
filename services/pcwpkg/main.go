package main // import "github.com/cisocrgroup/services/pcwpkg"

import (
	"flag"
	"math/rand"
	"net/http"
	"time"

	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	listen string
	dsn    string
	debug  bool
)

func init() {
	rand.Seed(time.Now().UnixNano()) // for random distribution of page ids
	flag.StringVar(&listen, "listen", ":80", "set listening host")
	flag.StringVar(&dsn, "dsn", "user:pass@proto(host)/dbname", "set mysql connection DSN")
	flag.BoolVar(&debug, "debug", false, "enable debug logging")
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
	routes()
	must(http.ListenAndServe(listen, nil))
}
