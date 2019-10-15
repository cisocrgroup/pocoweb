package main

import (
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type elRunner struct {
	project *db.Project
}

func (r elRunner) Name() string {
	return "calculating extended lexicon"
}

func (r elRunner) BookID() int {
	return r.project.BookID
}

func (r elRunner) Run(ctx context.Context) error {
	if err := r.setupWorkspace(); err != nil {
		return fmt.Errorf(
			"cannot calculate extended lexicon: %v", err)
	}
	if err := r.runEL(ctx); err != nil {
		return fmt.Errorf(
			"cannot calculate extended lexicon: %v", err)
	}
	return nil

}

func (r elRunner) setupWorkspace() error {
	doc, err := loadDocument(r.project)
	if err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	if err := doc.write(); err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	return nil
}

func (r elRunner) runEL(ctx context.Context) error {
	err := jobs.Run(
		ctx,
		"/apps/run_el.bash",
		config,
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
		filepath.Join(baseDir, r.project.Directory, "postcorrection", "el.json"),
	)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_el.bash: %v", err)
	}
	const stmnt = "UPDATE " + db.BooksTableName + " SET extendedlexicon=? WHERE bookid=?"
	_, err = db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_le.bash: %v", err)
	}
	service.UncacheProject(r.project)
	return nil
}

type rrdmRunner struct {
	project *db.Project
}

func (r rrdmRunner) Name() string {
	return "calculating post-correction"
}

func (r rrdmRunner) BookID() int {
	return r.project.BookID
}

func (r rrdmRunner) Run(ctx context.Context) error {
	protocol := filepath.Join(baseDir, r.project.Directory, "postcorrection", "rrdm.json")
	err := jobs.Run(
		ctx,
		"/apps/run_rrdm.bash",
		config,
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
		protocol,
	)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_rrdm.bash: %v", err)
	}
	if err := r.correct(protocol); err != nil {
		return fmt.Errorf("cannot correct: %v", err)
	}
	service.UncacheProject(r.project)
	return nil
}

type rrdmPVal struct {
	Normalized string  `json:"normalized"`
	OCR        string  `json:"ocr"`
	Cor        string  `json:"cor"`
	Confidence float64 `json:"confidence"`
	Taken      bool    `json:"taken"`
}

type rrdmP struct {
	Corrections map[string]rrdmPVal `json:"corrections"`
}

func (r rrdmRunner) correct(protocol string) (err error) {
	corrections, err := r.readProtocol(protocol)
	if err := r.correctInBackend(corrections); err != nil {
		return fmt.Errorf("cannot correct in backend: %v", err)
	}
	pc, err := r.correctInDatabase(corrections)
	if err != nil {
		return fmt.Errorf("cannot correct in database: %v", err)
	}
	if err := r.updateStatus(); err != nil {
		return fmt.Errorf("cannot update project status: %v", err)
	}
	if err := r.writeProtocol(pc, protocol); err != nil {
		return fmt.Errorf("cannot write protocol: %v", err)
	}
	return nil
}

func (r rrdmRunner) updateStatus() error {
	const stmnt = "UPDATE " + db.BooksTableName + " SET postcorrected=? WHERE bookid=?"
	_, err := db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot execute database update: %v", err)
	}
	return nil
}

func (r rrdmRunner) deleteCorrections() error {
	const del = "DELETE FROM autocorrections WHERE bookid = ?"
	_, err := db.Exec(service.Pool(), del, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot delete old corrections from database: %v", err)
	}
	return nil
}

func (r rrdmRunner) readProtocol(path string) (map[string]rrdmPVal, error) {
	var corrections rrdmP
	in, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("cannot open protocol: %v", err)
	}
	defer in.Close()
	if err := json.NewDecoder(in).Decode(&corrections); err != nil {
		return nil, fmt.Errorf("cannot decode protocol: %v", err)
	}
	return corrections.Corrections, nil
}

func (r rrdmRunner) writeProtocol(pc *api.PostCorrection, path string) (err error) {
	path = strings.Replace(path, ".json", "-pcw.json", 1)
	out, err := os.Create(path)
	if err != nil {
		return fmt.Errorf("cannot open post correction: %v", err)
	}
	defer func() {
		e := out.Close()
		if err == nil {
			err = e
		}
	}()
	if err := json.NewEncoder(out).Encode(pc); err != nil {
		return fmt.Errorf("cannot encode post correction: %v", err)
	}
	return nil
}

func (r rrdmRunner) correctInBackend(corrections map[string]rrdmPVal) error {
	// We should be communicating within docker compose - so skip verify is ok.
	client := api.NewClient(pocowebURL, true)
	for k, v := range corrections {
		log.Debugf("correction: %s %v", k, v)
		if !v.Taken { // only correct corrections that should be taken
			continue
		}
		var bid, pid, lid, tid int
		if _, err := fmt.Sscanf(k, "%d:%d:%d:%d", &bid, &pid, &lid, &tid); err != nil {
			return fmt.Errorf("invalid protocol id: %s", k)
		}
		_, err := client.PostTokenLen(bid, pid, lid, tid, len([]rune(v.OCR)), v.Cor)
		if err != nil {
			return fmt.Errorf("cannot post correct %s: %v", v.Normalized, err)
		}
	}
	return nil
}

func (r rrdmRunner) correctInDatabase(corrections map[string]rrdmPVal) (*api.PostCorrection, error) {
	if err := r.deleteCorrections(); err != nil {
		return nil, err
	}
	ins, err := service.Pool().Prepare("INSERT INTO autocrrections" +
		"(bookid,pageid,lineid,tokenid,ocrtypid,cortypid,taken) " +
		"VALUES(?,?,?,?,?,?,?,?)")
	if err != nil {
		return nil, fmt.Errorf("cannot prepare insert statement: %v", err)
	}
	defer ins.Close()
	t, err := newTypeInserter(service.Pool())
	if err != nil {
		return nil, fmt.Errorf("cannot insert types: %v", err)
	}
	defer t.Close()

	// insert corrections
	tokens := make(map[string]struct{ t, r int })
	for k, v := range corrections {
		var bid, pid, lid, tid int
		if _, err := fmt.Sscanf(k, "%d:%d:%d:%d", &bid, &pid, &lid, &tid); err != nil {
			return nil, fmt.Errorf("invalid protocol id: %s", k)
		}
		ocrtypid, err := t.id(v.Normalized)
		if err != nil {
			return nil, fmt.Errorf("cannot insert ocr type: %v", err)
		}
		cortypid, err := t.id(v.Cor)
		if err != nil {
			return nil, fmt.Errorf("cannot insert cor type: %v", err)
		}
		if _, err := ins.Exec(bid, pid, lid, tid, ocrtypid, cortypid, v.Taken); err != nil {
			return nil, fmt.Errorf("cannot insert correction: %v", err)
		}
		x := tokens[v.Normalized]
		if v.Taken {
			tokens[v.Normalized] = struct{ t, r int }{x.t + 1, x.r}
		} else {
			tokens[v.Normalized] = struct{ t, r int }{x.t, x.r + 1}
		}
	}
	return r.makePostCorrections(tokens), nil
}

func (r rrdmRunner) makePostCorrections(tokens map[string]struct{ t, r int }) *api.PostCorrection {
	pc := api.PostCorrection{
		BookID:    r.project.BookID,
		ProjectID: r.project.ProjectID,
		Always:    make(map[string]int),
		Never:     make(map[string]int),
		Sometimes: make(map[string]int),
	}
	// build protocol
	for k, v := range tokens {
		if v.t > 0 && v.r > 0 {
			pc.Sometimes[k] = v.t + v.r
		} else if v.t > 0 {
			pc.Always[k] = v.t
		} else {
			pc.Never[k] = v.r
		}
	}
	return &pc
}

type typeInserter struct {
	sel, ins *sql.Stmt
	ids      map[string]int
}

func newTypeInserter(dtb db.DB) (*typeInserter, error) {
	sel, err := dtb.Prepare("SELECT id FROM types WHERE  typ=?")
	if err != nil {
		return nil, fmt.Errorf("cannot prepare type select statement: %v", err)
	}
	ins, err := dtb.Prepare("INSERT into types (typ) VALUES (?)")
	if err != nil {
		sel.Close() // sel must be closed
		return nil, fmt.Errorf("cannot prepare type insert statment: %v", err)
	}
	return &typeInserter{
		sel: sel,
		ins: ins,
		ids: make(map[string]int),
	}, nil
}

func (t *typeInserter) id(typ string) (int, error) {
	if id, ok := t.ids[typ]; ok {
		return int(id), nil
	}
	rows, err := t.sel.Query(typ)
	if err != nil {
		return 0, fmt.Errorf("cannot query type %s: %v", typ, err)
	}
	defer rows.Close()
	if rows.Next() {
		var id int
		if err := rows.Scan(&id); err != nil {
			return 0, fmt.Errorf("cannot scan id for type %s: %v", typ, err)
		}
		t.ids[typ] = id
		return id, nil
	}
	res, err := t.ins.Exec(typ)
	if err != nil {
		return 0, fmt.Errorf("cannot insert type %s: %v", typ, err)
	}
	id, err := res.LastInsertId()
	if err != nil {
		return 0, fmt.Errorf("cannot insert type %s: %v", typ, err)
	}
	t.ids[typ] = int(id)
	return int(id), nil
}

func (t *typeInserter) Close() (err error) {
	defer func() {
		e := t.ins.Close()
		if err == nil {
			err = e
		}
	}()
	defer func() {
		e := t.sel.Close()
		if err == nil {
			err = e
		}
	}()
	return nil
}
