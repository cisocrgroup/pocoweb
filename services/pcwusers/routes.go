package main

import (
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type key int

var userKey key

type server struct {
	router *http.ServeMux
	pool   *sql.DB
}

func (s *server) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	s.router.ServeHTTP(w, r)
}

func (s *server) routes() {
	s.router = http.DefaultServeMux
	s.router.HandleFunc("/users", service.WithLog(service.WithMethods(
		http.MethodPost, s.withPostUser(s.handlePostUser()),
		http.MethodGet, s.handleGetAllUsers())))
	s.router.HandleFunc("/users/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithUserID(s.handleGetUser()),
		http.MethodPut, s.withPostUser(service.WithUserID(s.handlePutUser())),
		http.MethodDelete, service.WithUserID(s.handleDeleteUser()))))
}

func (s *server) withPostUser(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		var data api.CreateUserRequest
		if err := json.NewDecoder(r.Body).Decode(&data); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot read user: invalid data: %v", err)
			return
		}
		log.Debugf("withPostUser: %s", data.User)
		f(context.WithValue(ctx, userKey, data), w, r)
	}
}

func (s *server) handlePostUser() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		t := db.NewTransaction(s.pool.Begin())
		u := ctx.Value(userKey).(api.CreateUserRequest)
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

func (s *server) handleGetAllUsers() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		log.Debugf("get all users")
		users, err := db.FindAllUsers(s.pool)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot list users: %v", err)
			return
		}
		service.JSONResponse(w, api.Users{Users: users})
	}
}

func (s *server) handleGetUser() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		u, found, err := db.FindUserByID(s.pool, int64(service.UserIDFromCtx(ctx)))
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

func (s *server) handlePutUser() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		// this must not fail
		u := ctx.Value(userKey).(api.CreateUserRequest)
		t := db.NewTransaction(s.pool.Begin())
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

func (s *server) handleDeleteUser() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		uid := service.UserIDFromCtx(ctx)
		// you cannot delete users that still own books
		if s.ownsBooks(uid) {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot delete user %d: still owns books", uid)
			return
		}
		t := db.NewTransaction(s.pool.Begin())
		t.Do(func(dtb db.DB) error {
			if err := s.reassignPackagesToOwner(uid); err != nil {
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

func (s *server) ownsBooks(uid int) bool {
	sel := "SELECT * FROM projects where owner=? and id=origin"
	rows, err := db.Query(s.pool, sel, uid)
	if err != nil {
		log.Infof("cannot query for projects: %v", err)
		// we don't know if the user owns projects;
		// we assume that he might.
		return true
	}
	defer rows.Close()
	return rows.Next()
}

func (s *server) reassignPackagesToOwner(id int) error {
	sel := "SELECT p1.id,p2.owner " +
		"FROM projects p1 " +
		"JOIN projects p2 ON p1.origin=p2.id " +
		"WHERE p1.owner=?"
	rows, err := db.Query(s.pool, sel, id)
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
		_, err := db.Exec(s.pool, reassign, originowner, packageid)
		if err != nil {
			return fmt.Errorf("cannot reassign %d to %d: %v",
				packageid, originowner, err)
		}
	}
	return nil
}

func (s *server) insertRoot(root api.User, pass string) error {
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
	if err := db.SetUserPassword(s.pool, root, pass); err != nil {
		return fmt.Errorf("cannot set password for %s: %v", root, err)
	}
	return nil
}
