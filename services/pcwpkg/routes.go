package main

import (
	"encoding/json"
	"net/http"
	"strconv"

	"github.com/finkf/pcwgo/service"
)

func routes() {
	http.HandleFunc("/pkg/split/books/", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithAuth(service.WithProject(
			onlyProjects(withPostSplit(splitHandler())))))))
	http.HandleFunc("/pkg/assign/books/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(service.WithProject(
			onlyPackages(assignHandler()))))))
	http.HandleFunc("/pkg/takeback/books/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(service.WithProject(
			onlyProjects(takeBackHandler()))))))
}

func onlyProjects(f service.HandlerFunc) service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		if c.Project.BookID != c.Project.ProjectID { // project is a package
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: package id given")
			return
		}
		f(w, r, c)
	}
}

func onlyPackages(f service.HandlerFunc) service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		if c.Project.BookID == c.Project.ProjectID { // project is a book
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: project id given")
			return
		}
		f(w, r, c)
	}
}

type splitProjectRequest struct {
	UserIDs []int `json:"userIds"`
	Random  bool  `json:"random"`
}

func withPostSplit(f service.HandlerFunc) service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		var post splitProjectRequest
		if err := json.NewDecoder(r.Body).Decode(&post); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: invalid post data")
			return
		}
		if len(post.UserIDs) == 0 {
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: missing user ids")
			return
		}
		c.Post = post
		f(w, r, c)
	}
}

func splitHandler() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		pkgs, err := split(c.Project.BookID, c.Post.(splitProjectRequest))
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle split: %v", err)
			return
		}
		service.JSONResponse(w, pkgs)
	}
}

func assignHandler() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		str := r.URL.Query().Get("assignto")
		// no assignto -> assign package to original owner
		if str == "" {
			if err := assignToOriginalOwner(c.Project.ProjectID); err != nil {
				service.ErrorResponse(w, http.StatusInternalServerError,
					"internal server error: cannot handle assignment: %v", err)
				return
			}
			w.WriteHeader(http.StatusOK)
			return
		}
		// assignto -> user must be root
		if !c.Session.User.Admin {
			service.ErrorResponse(w, http.StatusForbidden,
				"forbidden: not an administrator account")
			return
		}
		// assignto -> check for valid userid
		assignto, err := strconv.Atoi(str)
		if err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: invalid user id: %s", str)
			return
		}
		if err := assignToUser(c.Project.ProjectID, assignto); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle assignment: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}

func takeBackHandler() service.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request, c *service.Data) {
		if err := takeBack(c.Project.ProjectID, int(c.Session.User.ID)); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle take back: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}
