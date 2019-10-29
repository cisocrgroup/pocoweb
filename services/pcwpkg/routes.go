package main

import (
	"context"
	"encoding/json"
	"net/http"
	"strconv"

	"github.com/finkf/pcwgo/service"
)

type key int

var postKey key

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
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := service.ProjectFromCtx(ctx)
		if p.BookID != p.ProjectID { // project is a package
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: package id given")
			return
		}
		f(ctx, w, r)
	}
}

func onlyPackages(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := service.ProjectFromCtx(ctx)
		if p.BookID == p.ProjectID { // project is a book
			service.ErrorResponse(w, http.StatusBadRequest,
				"bad request: project id given")
			return
		}
		f(ctx, w, r)
	}
}

type splitProjectRequest struct {
	UserIDs []int `json:"userIds"`
	Random  bool  `json:"random"`
}

func withPostSplit(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
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
		f(context.WithValue(ctx, postKey, postKey), w, r)
	}
}

func splitHandler() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		post := ctx.Value(postKey).(splitProjectRequest)
		p := service.ProjectFromCtx(ctx)
		pkgs, err := split(p.BookID, post)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle split: %v", err)
			return
		}
		service.JSONResponse(w, pkgs)
	}
}

func assignHandler() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		str := r.URL.Query().Get("assignto")
		p := service.ProjectFromCtx(ctx)
		// no assignto -> assign package to original owner
		if str == "" {
			if err := assignToOriginalOwner(p.ProjectID); err != nil {
				service.ErrorResponse(w, http.StatusInternalServerError,
					"internal server error: cannot handle assignment: %v", err)
				return
			}
			w.WriteHeader(http.StatusOK)
			return
		}
		auth := service.AuthFromCtx(ctx)
		// assignto -> user must be root
		if !auth.User.Admin {
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
		if err := assignToUser(p.ProjectID, assignto); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle assignment: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}

func takeBackHandler() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		p := service.ProjectFromCtx(ctx)
		if err := takeBack(p.ProjectID, int(auth.User.ID)); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"internal server error: cannot handle take back: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}
