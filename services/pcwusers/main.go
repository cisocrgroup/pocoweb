package main // import "github.com/finkf/pcwusers"

import (
	"database/sql"
	"flag"
	"net/http"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	root   = api.User{Admin: true}
	listen string
	dsn    string
	pass   string
	debug  bool
)

func init() {
	flag.StringVar(&listen, "listen", ":8080", "set listening host")
	flag.StringVar(&dsn, "dsn", "", "set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.StringVar(&root.Name, "root-name", "", "user name for the root account")
	flag.StringVar(&root.Email, "root-email", "", "email for the root account")
	flag.StringVar(&pass, "root-password", "", "password for the root account")
	flag.StringVar(&root.Institute, "root-institute", "", "institute for the root account")
	flag.BoolVar(&debug, "debug", false, "enable debug logging")
}

func must(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func main() {
	// flags
	flag.Parse()
	// database
	must(service.InitDebug(dsn, debug))
	defer service.Close()
	s := server{pool: service.Pool().(*sql.DB)}
	// root
	if root.Name != "" && root.Email != "" && pass != "" {
		must(s.insertRoot(root, pass))
	}
	s.routes()
	log.Infof("listening on %s", listen)
	must(http.ListenAndServe(listen, &s))
}
