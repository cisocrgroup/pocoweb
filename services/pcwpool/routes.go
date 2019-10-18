package main

import (
	"context"
	"database/sql"
	"net/http"

	"github.com/finkf/pcwgo/service"
)

type server struct {
	router *http.ServeMux
	pool   *sql.DB
}

func (s *server) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	s.router.ServeHTTP(w, r)
}

func (s *server) routes() {
	s.router = http.DefaultServeMux
	s.router.HandleFunc("/global", service.WithLog(service.WithMethods(
		http.MethodGet, s.handleGetGlobalPool())))
	s.router.HandleFunc("/users/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithIDs(s.handleGetUserPool(), "users"))))
}

func (s *server) handleGetUserPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
	}
}
func (s *server) handleGetGlobalPool() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
	}
}
