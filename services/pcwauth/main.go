package main // import "github.com/finkf/pcwauth"

import (
	"context"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"net/http"
	"net/http/httputil"
	"net/url"
	"regexp"
	"strconv"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	listen         string
	cert           string
	key            string
	dsn            string
	pocoweb        string
	profiler       string
	users          string
	postcorrection string
	pkg            string
	pool           string
	debug          bool
)

func init() {
	flag.StringVar(&listen, "listen", ":8080", "set listening host")
	flag.StringVar(&cert, "cert", "", "set cert file (no tls if omitted)")
	flag.StringVar(&key, "key", "", "set key file (no tls if omitted)")
	flag.StringVar(&dsn, "dsn", "",
		"set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.StringVar(&pocoweb, "pocoweb", "", "set host of pocoweb")
	flag.StringVar(&profiler, "profiler", "", "set host of pcwprofiler")
	flag.StringVar(&users, "users", "", "set host of pcwusers")
	flag.StringVar(&postcorrection, "postcorrection", "",
		"set host of pcwpostcorrection")
	flag.StringVar(&pkg, "pkg", "", "set host of pkg")
	flag.StringVar(&pool, "pool", "", "set host of pcwpool service")
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
	// proxies
	url, err := url.Parse(pocoweb)
	must(err)
	pocowebProxy := httputil.NewSingleHostReverseProxy(url)
	url, err = url.Parse(profiler)
	must(err)
	profilerProxy := httputil.NewSingleHostReverseProxy(url)
	url, err = url.Parse(users)
	must(err)
	usersProxy := httputil.NewSingleHostReverseProxy(url)
	url, err = url.Parse(pkg)
	must(err)
	pkgProxy := httputil.NewSingleHostReverseProxy(url)
	url, err = url.Parse(postcorrection)
	must(err)
	pcProxy := httputil.NewSingleHostReverseProxy(url)
	url, err = url.Parse(pool)
	must(err)
	poolProxy := httputil.NewSingleHostReverseProxy(url)

	// login
	http.HandleFunc(api.LoginURL,
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithAuth(getLogin()),
			http.MethodPost, postLogin())))
	http.HandleFunc(api.LogoutURL, service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(getLogout()))))
	// jobs
	http.HandleFunc("/jobs/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(service.WithJobID(getJob())))))
	// postcorrection
	http.HandleFunc("/postcorrect/le/books/", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithAuth(
			service.WithProject(projectOwner(proxy(pcProxy)))),
		http.MethodGet, service.WithAuth(
			service.WithProject(projectOwner(proxy(pcProxy)))),
		http.MethodPut, service.WithAuth(
			service.WithProject(projectOwner(proxy(pcProxy)))))))
	http.HandleFunc("/postcorrect/books/", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithAuth(
			service.WithProject(projectOwner(proxy(pcProxy)))),
		http.MethodGet, service.WithAuth(
			service.WithProject(projectOwner(proxy(pcProxy)))))))
	http.HandleFunc("/postcorrect/models/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(service.WithProject(projectOwner(proxy(pcProxy)))),
		http.MethodPost, service.WithAuth(service.WithProject(projectOwner(proxy(pcProxy)))))))
	// pkg
	http.HandleFunc("/pkg/", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithAuth(
			service.WithProject(projectOwner(proxy(pkgProxy)))),
		http.MethodGet, service.WithAuth(
			service.WithProject(projectOwner(proxy(pkgProxy)))))))
	// pool
	http.HandleFunc("/pool/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(root(withUserid(proxy(poolProxy)))))))
	// user management
	http.HandleFunc("/users", service.WithLog(service.WithMethods(
		http.MethodPost, service.WithAuth(root(proxy(usersProxy))),
		http.MethodGet, service.WithAuth(root(proxy(usersProxy))))))
	http.HandleFunc("/users/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(rootOrSelf(proxy(usersProxy))),
		http.MethodPut, service.WithAuth(rootOrSelf(proxy(usersProxy))),
		http.MethodDelete, service.WithAuth(rootOrSelf(proxy(usersProxy))))))
	// book management
	http.HandleFunc("/books", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(withUserid(proxy(pocowebProxy))),
		http.MethodPost, service.WithAuth(withUserid(proxy(pocowebProxy))))))
	http.HandleFunc("/books/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(
			service.WithProject(projectOwner(proxy(pocowebProxy)))),
		http.MethodPut, service.WithAuth(
			service.WithProject(projectOwner(proxy(pocowebProxy)))),
		http.MethodPost, service.WithAuth(
			service.WithProject(projectOwner(proxy(pocowebProxy)))),
		http.MethodDelete, service.WithAuth(
			service.WithProject(projectOwner(proxy(pocowebProxy)))))))
	// version
	http.HandleFunc("/api-version",
		service.WithMethods(http.MethodGet, proxy(pocowebProxy)))
	// profiling
	http.HandleFunc("/profile/languages", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(proxy(profilerProxy)))))
	http.HandleFunc("/profile/jobs/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(proxy(profilerProxy)))))
	http.HandleFunc("/profile/", service.WithLog(service.WithMethods(
		http.MethodGet, service.WithAuth(
			service.WithProject(projectOwner(proxy(profilerProxy)))),
		http.MethodPost, service.WithAuth(
			service.WithProject(projectOwner(proxy(profilerProxy)))))))
	log.Infof("listening on %s", listen)
	if cert != "" && key != "" {
		must(http.ListenAndServeTLS(listen, cert, key, nil))
	} else {
		must(http.ListenAndServe(listen, nil))
	}
}

func root(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		log.Debugf("root: %s", auth.User)
		if !auth.User.Admin {
			service.ErrorResponse(w, http.StatusForbidden,
				"only root allowed to access: %s", auth.User)
			return
		}
		f(ctx, w, r)
	}
}

func rootOrSelf(f service.HandlerFunc) service.HandlerFunc {
	re := regexp.MustCompile(`/users/(\d+)`)
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		log.Debugf("rootOrSelf: %s", auth.User)
		var uid int
		if n := service.ParseIDs(r.URL.String(), re, &uid); n == 0 {
			service.ErrorResponse(w, http.StatusNotFound,
				"cannot find: %s", r.URL.String())
			return
		}
		log.Debugf("user id: %d", uid)
		if !auth.User.Admin && int64(uid) != auth.User.ID {
			service.ErrorResponse(w, http.StatusForbidden,
				"not allowed to access: %s", auth.User)
			return
		}
		f(ctx, w, r)
	}
}

func projectOwner(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		p := service.ProjectFromCtx(ctx)
		log.Debugf("projectOwner: id: %d, user: %s, owner: %s",
			p.ProjectID, auth.User, p.Owner)
		if auth.User.ID != p.Owner.ID {
			service.ErrorResponse(w, http.StatusForbidden,
				"not allowed to access project: %d", p.ProjectID)
			return
		}
		f(ctx, w, r)
	}
}

func postLogin() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		var data api.LoginRequest
		if err := json.NewDecoder(r.Body).Decode(&data); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot login: invalid login data")
			return
		}

		user, found, err := db.FindUserByEmail(service.Pool(), data.Email)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot login: %v", err)
			return
		}
		if !found {
			service.ErrorResponse(w, http.StatusNotFound,
				"cannot login: no such user: %s", data.Email)
			return
		}

		log.Infof("login request for user: %s", user)
		if err = db.AuthenticateUser(service.Pool(), user, data.Password); err != nil {
			service.ErrorResponse(w, http.StatusUnauthorized,
				"cannot login: invalid password: %v", err)
			return
		}
		if err = db.DeleteSessionByUserID(service.Pool(), user.ID); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot login: cannot delete session: %v", err)
			return
		}

		s, err := db.InsertSession(service.Pool(), user)
		log.Debugf("login: new session: %s", s)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot login: cannot insert session: %v", err)
			return
		}
		service.JSONResponse(w, s)
	}
}

func getLogin() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		service.JSONResponse(w, auth)
	}
}

func getLogout() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		auth := service.AuthFromCtx(ctx)
		log.Debugf("logout session: %s", auth)
		db.Exec(service.Pool(), "remove from sessions where auth=?", auth.Auth)
		if err := db.DeleteSessionByUserID(service.Pool(), auth.User.ID); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot logout: cannot delete session: %v", err)
			return
		}
		w.WriteHeader(http.StatusOK)
	}
}

func getJob() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		jobID := service.JobIDFromCtx(ctx)
		status, err := findJob(jobID)
		// return not running job if the job does not exist
		// to indicate that it is ok to run a job
		if err == errJobNotFound {
			service.JSONResponse(w, api.JobStatus{
				JobID:      jobID,
				BookID:     jobID,
				StatusID:   db.StatusIDEmpty,
				StatusName: db.StatusEmpty,
			})
			return
		}
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot get job status: %v", err)
			return
		}
		service.JSONResponse(w, status)
	}
}

var errJobNotFound = errors.New("job not found")

func findJob(jobID int) (*api.JobStatus, error) {
	// search for the project id mapped to the job/book-ID
	// (project.origin is joined with jobid in the view)
	stmt := `select * from job_status_project_view where projectid=?`
	rows, err := db.Query(service.Pool(), stmt, jobID)
	if err != nil {
		return nil, fmt.Errorf("cannot query job id %d: %v", jobID, err)
	}
	defer rows.Close()
	if !rows.Next() {
		return nil, errJobNotFound
	}
	var sid, jid, pid int
	var ts int64
	var jname, sname string
	if err := rows.Scan(&jid, &sid, &jname, &ts, &sname, &pid); err != nil {
		return nil, fmt.Errorf("cannot scan job id %d: %v", jobID, err)
	}
	return &api.JobStatus{
		JobID:      jid,
		BookID:     jid, // always equal to the bookID
		StatusID:   sid,
		StatusName: sname,
		JobName:    jname,
		Timestamp:  ts,
		// TODO: ProjectID:  pid,
	}, nil
}

func withUserid(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		q := r.URL.Query()
		auth := service.AuthFromCtx(ctx)
		q.Set("userid", strconv.Itoa(int(auth.User.ID)))
		r.URL.RawQuery = q.Encode()
		f(ctx, w, r)
	}
}

func proxy(p *httputil.ReverseProxy) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		log.Infof("proxy: %s %s", r.Method, r.URL.String())
		p.ServeHTTP(w, r)
	}
}
