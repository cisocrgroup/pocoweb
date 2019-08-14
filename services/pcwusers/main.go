package main // import "github.com/finkf/pcwusers"

import (
	"encoding/json"
	"flag"
	"fmt"
	"net/http"
	"regexp"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	listen  string
	dsn     string
	rName   string
	rPass   string
	rEmail  string
	rInst   string
	debug   bool
	usersre = regexp.MustCompile(`/users/(\d+)`)
)

func init() {
	flag.StringVar(&listen, "listen", ":8080", "set listening host")
	flag.StringVar(&dsn, "dsn", "", "set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.StringVar(&rName, "root-name", "", "user name for the root account")
	flag.StringVar(&rEmail, "root-email", "", "email for the root account")
	flag.StringVar(&rPass, "root-password", "", "password for the root account")
	flag.StringVar(&rInst, "root-institute", "", "institute for the root account")
	flag.BoolVar(&debug, "debug", false, "enable debug logging")
}

func must(err error) {
	if err != nil {
		log.Fatalf("error: %v", err)
	}
}

func insertRoot() error {
	root := api.User{
		Name:      rName,
		Email:     rEmail,
		Institute: rInst,
		Admin:     true,
	}
	_, found, err := db.FindUserByEmail(service.Pool(), root.Email)
	if err != nil {
		return fmt.Errorf("cannot find user %s: %v", root, err)
	}
	if found { // root allready exists
		return nil
	}
	if err = db.InsertUser(service.Pool(), &root); err != nil {
		return fmt.Errorf("cannot create user %s: %v", root, err)
	}
	if err := db.SetUserPassword(service.Pool(), root, rPass); err != nil {
		return fmt.Errorf("cannot set password for %s: %v", root, err)
	}
	return nil
}

func main() {
	// flags
	flag.Parse()
	if debug {
		log.SetLevel(log.DebugLevel)
	}
	// database
	must(service.Init(dsn))
	defer service.Close()
	// root
	if rName != "" && rEmail != "" && rPass != "" {
		must(insertRoot())
	}
	// handlers
	http.HandleFunc("/users", service.WithLog(service.WithMethods(
		http.MethodPost, withPostUser(postUser()),
		http.MethodGet, getAllUsers())))
	http.HandleFunc("/users/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithIDs(getUser(), "users"),
		http.MethodPut, withPostUser(service.WithIDs(putUser(), "users")),
		http.MethodDelete, service.WithIDs(deleteUser(), "users"))))
	log.Infof("listening on %s", listen)
	must(http.ListenAndServe(listen, nil))
}

func withPostUser(f service.HandlerFunc) service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		var data api.CreateUserRequest
		if err := json.NewDecoder(r.Body).Decode(&data); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot read user: invalid data: %v", err)
			return
		}
		log.Debugf("withPostUser: %s", data.User)
		d.Post = data
		f(w, r, d)
	}
}

func getAllUsers() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		log.Debugf("get all users")
		users, err := db.FindAllUsers(service.Pool())
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot list users: %v", err)
			return
		}
		service.JSONResponse(w, api.Users{Users: users})
	}
}

func getUser() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		u, found, err := db.FindUserByID(service.Pool(), int64(d.IDs["users"]))
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot get user: %v", err)
			return
		}
		if !found {
			service.ErrorResponse(w, http.StatusNotFound,
				"cannot get user: not found")
			return
		}
		log.Printf("get user: %s", u)
		service.JSONResponse(w, u)
	}
}

func postUser() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		t := db.NewTransaction(service.Pool().Begin())
		u := d.Post.(api.CreateUserRequest)
		t.Do(func(dtb db.DB) error {
			if err := db.InsertUser(dtb, &u.User); err != nil {
				return fmt.Errorf("cannot insert user into database: %v", err)
			}
			if err := db.SetUserPassword(dtb, u.User, u.Password); err != nil {
				return fmt.Errorf("cannot set password: %v", err)
			}
			return nil
		})
		if err := t.Done(); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest, "cannot create user: %v", err)
			return
		}
		service.JSONResponse(w, u.User)
	}
}

func putUser() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		// this must not fail
		u := d.Post.(api.CreateUserRequest)
		t := db.NewTransaction(service.Pool().Begin())
		t.Do(func(dtb db.DB) error {
			if err := db.UpdateUser(dtb, u.User); err != nil {
				return fmt.Errorf("cannot update user in database: %v", err)
			}
			if u.Password != "" { // do update password if not empty
				if err := db.SetUserPassword(dtb, u.User, u.Password); err != nil {
					return fmt.Errorf("cannot set password: %v", err)
				}
			}
			user, ok, err := db.FindUserByID(dtb, int64(u.User.ID))
			if err != nil {
				return fmt.Errorf("cannot find user: %v", err)
			}
			if !ok {
				return fmt.Errorf("cannot find user: invalid id: %d", u.User.ID)
			}
			u.User = user
			return nil
		})
		if err := t.Done(); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot update user: %v", err)
			return
		}
		service.JSONResponse(w, u.User)
	}
}

func deleteUser() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, d *service.Data) {
		uid := d.IDs["users"]
		// you cannot delete users that still own books
		if ownsBooks(service.Pool(), uid) {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot delete user %d: still owns books", uid)
			return
		}
		t := db.NewTransaction(service.Pool().Begin())
		t.Do(func(dtb db.DB) error {
			if err := reassignPackagesToOwner(dtb, uid); err != nil {
				return fmt.Errorf("cannot reassign packages: %v", err)
			}
			del := "delete from users where id=?"
			if _, err := db.Exec(dtb, del, uid); err != nil {
				return fmt.Errorf("cannot delete user %d: %v", uid, err)
			}
			return nil
		})
		if err := t.Done(); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot delete user: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}

func ownsBooks(dtb db.DB, id int) bool {
	sel := "SELECT * FROM projects where owner=? and id=origin"
	rows, err := db.Query(dtb, sel, id)
	if err != nil {
		log.Infof("cannot query for projects: %v", err)
		// we don't know if the user owns projects;
		// we assume that he might.
		return true
	}
	defer rows.Close()
	return rows.Next()
}

func reassignPackagesToOwner(dtb db.DB, id int) error {
	sel := "SELECT p1.id,p2.owner " +
		"FROM projects p1 " +
		"JOIN projects p2 ON p1.origin=p2.id " +
		"WHERE p1.owner=?"
	rows, err := db.Query(dtb, sel, id)
	if err != nil {
		return fmt.Errorf("cannot query projects: %v", err)
	}
	defer rows.Close()
	reassign := "UPDATE projects SET owner=? WHERE id=?"
	for rows.Next() {
		var packageid, originowner int
		if err := rows.Scan(&packageid, &originowner); err != nil {
			return fmt.Errorf("cannot scan packageid and origin: %v", err)
		}
		if originowner == id {
			continue
		}
		_, err := db.Exec(dtb, reassign, originowner, packageid)
		if err != nil {
			return fmt.Errorf("cannot reassign %d to %d: %v",
				packageid, originowner, err)
		}
	}
	return nil
}
