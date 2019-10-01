package main // import "github.com/finkf/pcwprofiler"

import (
	"context"
	"encoding/json"
	"flag"
	"net/http"
	"path/filepath"
	"regexp"
	"strings"
	"unicode"

	"github.com/finkf/gofiler"
	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

var (
	listen      = ":80"
	baseDir     = "/"
	languageDir = "/language-data"
	profbin     = "/apps/profiler"
	dsn         = ""
	debug       = false
	cutoff      = 1e-4
)

func init() {
	flag.StringVar(&listen, "listen", listen, "set host")
	flag.StringVar(&baseDir, "base",
		baseDir, "set project base dir")
	flag.StringVar(&languageDir, "language-dir",
		languageDir, "set profiler's language backend")
	flag.StringVar(&profbin, "profiler",
		profbin, "path to profiler executable")
	flag.StringVar(&dsn, "dsn", dsn,
		"set mysql connection DSN (user:pass@proto(host)/dbname)")
	flag.Float64Var(&cutoff, "cutoff",
		cutoff, "set cutoff weight for profiler suggestions")
	flag.BoolVar(&debug, "debug", debug, "enable debugging")
}

func main() {
	// command line args
	flag.Parse()
	// init
	if err := service.InitDebug(dsn, debug); err != nil {
		log.Fatalf("cannot initialize service: %v", err)
	}
	defer service.Close()
	// start jobs
	jobs.Init(service.Pool())
	defer jobs.Close()
	// start server
	http.HandleFunc("/profile/languages",
		service.WithLog(service.WithMethods(http.MethodGet, getLanguages())))
	http.HandleFunc("/profile/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getProfile()),
			http.MethodPost, withAdditionalLexicon(service.WithProject(run())))))
	http.HandleFunc("/profile/patterns/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getPatterns()))))
	http.HandleFunc("/profile/suspicious/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getSuspiciousWords()))))
	http.HandleFunc("/profile/adaptive/books/",
		service.WithLog(service.WithMethods(
			http.MethodGet, service.WithProject(getAdaptiveTokens()))))
	log.Infof("listening on %s", listen)
	log.Fatal(http.ListenAndServe(listen, nil))
}

func getLanguages() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		configs, err := gofiler.ListLanguages(languageDir)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot list languages: %v", err)
			return
		}
		ls := api.Languages{Languages: make([]string, len(configs))}
		for i := range configs {
			ls.Languages[i] = configs[i].Language
		}
		service.JSONResponse(w, ls)
	}
}

func getProfile() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		q := r.URL.Query()["q"]
		p := ctx.Value("project").(*db.Project)
		if len(q) == 0 { // return the whole profile
			getWholeProfile(w, r, p)
			return
		}
		queryProfile(w, p, q)
	}
}

func getWholeProfile(w http.ResponseWriter, r *http.Request, p *db.Project) {
	w.Header().Add("Content-Encoding", "gzip")
	w.Header().Add("Content-Type", "application/json")
	http.ServeFile(w, r, filepath.Join(baseDir, p.Directory, "profile.json.gz"))
}

func queryProfile(w http.ResponseWriter, p *db.Project, qs []string) {
	var splitRe = regexp.MustCompile(`^(\p{P}*)(.*?)(\p{P}*)$`)
	ss := api.Suggestions{
		BookID:      p.BookID,
		Suggestions: make(map[string][]api.Suggestion),
	}
	for _, q := range qs {
		m := splitRe.FindStringSubmatch(q)
		if err := selectSuggestions(m[1], m[2], m[3], &ss); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot get suggestions: %v", err)
			return
		}
	}
	service.JSONResponse(w, ss)
}

func selectSuggestions(pre, q, suf string, ss *api.Suggestions) error {
	const stmt = "SELECT s.id,s.weight,s.distance,s.dict," +
		"s.histpatterns,s.ocrpatterns," +
		"s.topsuggestion,t1.typ,t2.typ,t3.typ " +
		"FROM suggestions s " +
		"JOIN types t1 ON s.tokentypid=t1.id " +
		"JOIN types t2 ON s.suggestiontypid=t2.id " +
		"JOIN types t3 ON s.moderntypid=t3.id " +
		"WHERE s.bookid=? AND t1.typ=?"
	rows, err := db.Query(service.Pool(), stmt, ss.BookID, strings.ToLower(q))
	if err != nil {
		return err
	}
	defer rows.Close()
	for rows.Next() {
		var h, o string
		var s api.Suggestion
		if err := rows.Scan(&s.ID, &s.Weight, &s.Distance, &s.Dict, &h, &o,
			&s.Top, &s.Token, &s.Suggestion, &s.Modern); err != nil {
			return err
		}
		s.HistPatterns = strings.Split(h, ",")
		s.OCRPatterns = strings.Split(o, ",")
		s.Token = pre + applyCasing(q, s.Token) + suf
		s.Suggestion = pre + applyCasing(q, s.Suggestion) + suf
		s.Modern = pre + applyCasing(q, s.Modern) + suf
		ss.Suggestions[q] = append(ss.Suggestions[q], s)
	}
	return nil
}

// Apply the casing of model to string.
func applyCasing(model, str string) string {
	wmodel := []rune(model)
	wstr := []rune(str)
	var isupper bool
	for i := range wstr {
		if i < len(wmodel) {
			isupper = unicode.IsUpper(wmodel[i])
		}
		if isupper {
			wstr[i] = unicode.ToUpper(wstr[i])
		}
	}
	return string(wstr)
}

func withAdditionalLexicon(f service.HandlerFunc) service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		var alex api.AdditionalLexicon
		if err := json.NewDecoder(r.Body).Decode(&alex); err != nil {
			service.ErrorResponse(w, http.StatusBadRequest,
				"cannot decode additional lexicon: %v", err)
			return
		}
		// d.Post = alex
		f(context.WithValue(ctx, "data", alex), w, r)
	}
}

func run() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := profiler{
			project: ctx.Value("project").(*db.Project),
			alex:    ctx.Value("data").(api.AdditionalLexicon),
		}
		jobID, err := jobs.Start(context.Background(), &p)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot run profiler: %v", err)
			return
		}
		service.JSONResponse(w, api.Job{ID: jobID})
	}
}

func getPatterns() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		qs := r.URL.Query()["q"]
		h := strings.ToLower(r.URL.Query().Get("ocr"))
		var ocr bool
		if h == "true" || h == "1" {
			ocr = true
		}
		if len(qs) == 0 {
			getAllPatterns(ctx, w, ocr)
			return
		}
		queryPatterns(ctx, w, qs, ocr)
	}
}

func getAllPatterns(ctx context.Context, w http.ResponseWriter, ocr bool) {
	const stmt = "SELECT p.pattern,COUNT(*) " +
		"FROM errorpatterns p " +
		"JOIN suggestions s ON s.id=p.suggestionid " +
		"WHERE p.bookID=? AND p.ocr=? AND s.topsuggestion=? " +
		"GROUP BY p.pattern"
	p := ctx.Value("project").(*db.Project)
	rows, err := db.Query(service.Pool(), stmt, p.BookID, ocr, true)
	if err != nil {
		service.ErrorResponse(w, http.StatusInternalServerError,
			"cannot get patterns: %v", err)
		return
	}
	defer rows.Close()
	patterns := api.PatternCounts{
		BookID:    p.BookID,
		ProjectID: p.ProjectID,
		OCR:       ocr,
		Counts:    make(map[string]int),
	}
	for rows.Next() {
		var p string
		var c int
		if err := rows.Scan(&p, &c); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot scan pattern: %v", err)
			return
		}
		patterns.Counts[p] = c
	}
	service.JSONResponse(w, patterns)
}

func queryPatterns(ctx context.Context, w http.ResponseWriter, qs []string, ocr bool) {
	const stmt = "SELECT p.pattern,s.id,s.weight,s.distance," +
		"s.dict,s.histpatterns,s.ocrpatterns,s.topsuggestion,t1.typ,t2.typ,t3.typ " +
		"FROM errorpatterns p " +
		"JOIN suggestions s ON p.suggestionID=s.id " +
		"JOIN types t1 ON s.tokentypid=t1.id " +
		"JOIN types t2 on s.suggestiontypid=t2.id " +
		"JOIN types t3 on s.moderntypid=t3.id " +
		"WHERE p.bookID=? AND p.pattern=? AND p.ocr=?"
	p := ctx.Value("project").(*db.Project)
	res := api.Patterns{
		BookID:    p.BookID,
		ProjectID: p.ProjectID,
		OCR:       ocr,
		Patterns:  make(map[string][]api.Suggestion),
	}
	for _, q := range qs {
		rows, err := db.Query(service.Pool(), stmt, p.BookID, q, ocr)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot query pattern %q: %v", q, err)
			return
		}
		defer rows.Close()
		for rows.Next() {
			var p, h, o string
			var s api.Suggestion
			if err := rows.Scan(&p, &s.ID, &s.Weight, &s.Distance, &s.Dict,
				&h, &o, &s.Top, &s.Token, &s.Suggestion, &s.Modern); err != nil {
				service.ErrorResponse(w, http.StatusInternalServerError,
					"cannot query pattern %q: %v", q, err)
				return
			}
			s.HistPatterns = strings.Split(h, ",")
			s.OCRPatterns = strings.Split(o, ",")
			res.Patterns[p] = append(res.Patterns[p], s)
		}
	}
	service.JSONResponse(w, res)
}

func getSuspiciousWords() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		const stmt = "SELECT t.typ,tc.counts " +
			"FROM suggestions s " +
			"JOIN types t ON s.tokentypid=t.id " +
			"JOIN typcounts tc ON s.tokentypid=tc.typid AND s.bookid=tc.bookid " +
			"WHERE s.bookID=? AND s.topsuggestion=true AND s.distance > 0"
		p := ctx.Value("project").(*db.Project)
		rows, err := db.Query(service.Pool(), stmt, p.BookID)
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot get suspicious words: %v", err)
			return
		}
		defer rows.Close()
		patterns := api.SuggestionCounts{
			BookID:    p.BookID,
			ProjectID: p.ProjectID,
			Counts:    make(map[string]int),
			// explicit map to avoid null entry in json
		}
		for rows.Next() {
			var p string
			var c int
			if err := rows.Scan(&p, &c); err != nil {
				service.ErrorResponse(w, http.StatusInternalServerError,
					"cannot get suspicious words: %v", err)
				return
			}
			patterns.Counts[p] = c
		}
		service.JSONResponse(w, patterns)
	}
}

func getAdaptiveTokens() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := ctx.Value("project").(*db.Project)
		at := api.AdaptiveTokens{
			BookID:         p.BookID,
			ProjectID:      p.ProjectID,
			AdaptiveTokens: []string{},
			// set explicitly to return an empty list
		}
		seen := make(map[string]bool)
		var i int
		eachLine(p.BookID, func(line db.Chars) error {
			eachWord(line, func(word db.Chars) error {
				if i < 10 {
					i++
				}
				if word.IsFullyCorrected() {
					str := strings.ToLower(word.Cor())
					if !seen[str] {
						at.AdaptiveTokens = append(at.AdaptiveTokens, str)
						seen[str] = true
					}
				}
				return nil
			})
			return nil
		})
		service.JSONResponse(w, at)
	}
}
