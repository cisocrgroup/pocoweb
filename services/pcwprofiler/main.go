package main // import "github.com/finkf/pcwprofiler"

import (
	"context"
	"database/sql"
	"encoding/json"
	"flag"
	"fmt"
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

type key int

var (
	listen                   = ":80"
	baseDir                  = "/"
	languageDir              = "/language-data"
	profbin                  = "/apps/profiler"
	dsn                      = ""
	debug                    = false
	cutoff                   = 1e-4
	additionalLexiconKey key = 0
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
		p := service.ProjectFromCtx(ctx)
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
		ProjectID:   p.ProjectID,
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
		s.HistPatterns = splitPatterns(h)
		s.OCRPatterns = splitPatterns(o)
		s.Token = pre + applyCasing(q, s.Token) + suf
		s.Suggestion = pre + applyCasing(q, s.Suggestion) + suf
		s.Modern = pre + applyCasing(q, s.Modern) + suf
		ss.Suggestions[q] = append(ss.Suggestions[q], s)
	}
	return nil
}

func splitPatterns(patterns string) []string {
	if patterns == "" {
		return []string{}
	}
	return strings.Split(patterns, ",")
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
		f(context.WithValue(ctx, additionalLexiconKey, alex), w, r)
	}
}

func run() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := profiler{
			project: service.ProjectFromCtx(ctx),
			alex:    ctx.Value(additionalLexiconKey).(api.AdditionalLexicon),
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
	const stmt = "SELECT p.pattern,t.typ " +
		"FROM errorpatterns p " +
		"JOIN suggestions s ON s.id=p.suggestionid " +
		"JOIN types t ON t.id=s.tokentypid " +
		"WHERE p.bookID=? AND p.ocr=? AND s.topsuggestion=?"
	project := service.ProjectFromCtx(ctx)
	rows, err := db.Query(service.Pool(), stmt, project.BookID, ocr, true)
	if err != nil {
		service.ErrorResponse(w, http.StatusInternalServerError,
			"cannot get patterns: %v", err)
		return
	}
	defer rows.Close()
	patterns := api.PatternCounts{
		BookID:    project.BookID,
		ProjectID: project.ProjectID,
		OCR:       ocr,
		Counts:    make(map[string]int), // prohibit json nil
	}
	// map tokens to patterns
	tokens2patterns := make(map[string]map[string]bool)
	for rows.Next() {
		var p, t string
		if err := rows.Scan(&p, &t); err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot scan pattern: %v", err)
			return
		}
		if _, ok := tokens2patterns[t]; !ok {
			tokens2patterns[t] = make(map[string]bool)
		}
		tokens2patterns[t][p] = true
	}
	err = eachOCRToken(project, func(token db.Chars) {
		str := strings.ToLower(token.Cor())
		if _, ok := tokens2patterns[str]; !ok {
			return
		}
		for p := range tokens2patterns[str] {
			patterns.Counts[p]++
		}
	})
	if err != nil {
		service.ErrorResponse(w, http.StatusInternalServerError,
			"cannot count error patterns: %v", err)
		return
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
	p := service.ProjectFromCtx(ctx)
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
		const stmt = "SELECT t.typ " +
			"FROM suggestions s " +
			"JOIN types t ON s.tokentypid=t.id " +
			"WHERE s.bookID=? AND s.topsuggestion=true AND s.distance > 0"
		p := service.ProjectFromCtx(ctx)
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
			if err := rows.Scan(&p); err != nil {
				service.ErrorResponse(w, http.StatusInternalServerError,
					"cannot get suspicious words: %v", err)
				return
			}
			patterns.Counts[p] = 0
		}
		err = eachOCRToken(p, func(token db.Chars) {
			str := strings.ToLower(token.Cor())
			if _, ok := patterns.Counts[str]; !ok {
				return
			}
			patterns.Counts[str]++
		})
		if err != nil {
			service.ErrorResponse(w, http.StatusInternalServerError,
				"cannot count suspicious words: %v", err)
			return
		}
		service.JSONResponse(w, patterns)
	}
}

func eachOCRToken(p *db.Project, f func(db.Chars)) error {
	pageIDs, err := selectPageIDs(p)
	if err != nil {
		return fmt.Errorf("cannot load page ids: %v", err)
	}
	stmnt, err := service.Pool().Prepare(`
SELECT c.lineid,c.cor,c.ocr,c.cut,c.conf,c.seq
FROM contents c
WHERE c.bookid=? and c.pageid=?
ORDER BY c.lineid,c.seq`)
	if err != nil {
		return fmt.Errorf("cannot prepare statement: %v", err)
	}
	defer stmnt.Close()
	for _, pageID := range pageIDs {
		rows, err := stmnt.Query(p.BookID, pageID)
		if err != nil {
			return fmt.Errorf("cannot execute statement: %v", err)
		}
		defer rows.Close()
		if err := eachOCRTokenInRows(rows, f); err != nil {
			return err
		}
	}
	return nil
}

func eachOCRTokenInRows(rows *sql.Rows, f func(db.Chars)) error {
	var line db.Chars
	var lineid int
	for rows.Next() {
		old := lineid
		var c db.Char
		if err := rows.Scan(&lineid, &c.Cor, &c.OCR, &c.Cut, &c.Conf, &c.Seq); err != nil {
			return fmt.Errorf("cannot scan character: %v", err)
		}
		// no new line; append and continue
		if old == lineid || len(line) == 0 {
			line = append(line, c)
			continue
		}
		// clear line and skip
		if line.IsFullyCorrected() {
			line = line[:0]
			continue
		}
		eachOCRTokenOnLine(line, f)
		line = append(line[:0], c)
	}
	// last line
	if len(line) > 0 && !line.IsFullyCorrected() {
		eachOCRTokenOnLine(line, f)
	}
	return nil
}

func eachOCRTokenOnLine(line db.Chars, f func(db.Chars)) {
	for t, rest := line.NextWord(); len(t) > 0; t, rest = rest.NextWord() {
		t = t.Trim(func(c db.Char) bool {
			r := c.OCR
			if c.Cor != 0 {
				r = c.Cor
			}
			return r == -1 || unicode.IsPunct(r)
		})
		if len(t) == 0 || t.IsFullyCorrected() {
			continue
		}
		f(t)
	}
}

func selectPageIDs(p *db.Project) ([]int, error) {
	const stmnt = "SELECT pageid FROM project_pages WHERE projectid=?"
	var ret []int
	rows, err := service.Pool().Query(stmnt, p.ProjectID)
	if err != nil {
		return nil, fmt.Errorf("cannot execute query: %v", err)
	}
	defer rows.Close()
	for rows.Next() {
		var id int
		if err := rows.Scan(&id); err != nil {
			return nil, fmt.Errorf("cannot scan id: %v", err)
		}
		ret = append(ret, id)
	}
	return ret, nil
}

func getAdaptiveTokens() service.HandlerFunc {
	return func(ctx context.Context, w http.ResponseWriter, r *http.Request) {
		p := service.ProjectFromCtx(ctx)
		at := api.AdaptiveTokens{
			BookID:         p.BookID,
			ProjectID:      p.ProjectID,
			AdaptiveTokens: []string{}, // set explicitly to return an empty list
		}
		seen := make(map[string]bool)
		eachLine(p.BookID, func(line db.Chars) error {
			eachWord(line, func(word db.Chars) error {
				if word.IsFullyCorrected() {
					str := strings.ToLower(trim(word).Cor())
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
