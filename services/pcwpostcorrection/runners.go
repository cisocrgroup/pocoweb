package main

import (
	"context"
	"database/sql"
	"encoding/json"
	"fmt"
	"math/rand"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/finkf/pcwgo/api"
	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
	log "github.com/sirupsen/logrus"
)

type leRunner struct {
	project *db.Project
}

func (r leRunner) Name() string {
	return "calculating extended lexicon"
}

func (r leRunner) BookID() int {
	return r.project.BookID
}

func (r leRunner) Run(ctx context.Context) error {
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

func (r leRunner) setupWorkspace() error {
	doc, err := loadDocument(r.project)
	if err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	if err := doc.write(); err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	return nil
}

func (r leRunner) runEL(ctx context.Context) error {
	const script = "/apps/run.bash"
	err := jobs.Run(
		ctx,
		script,
		"le",
		filepath.Join(baseDir, r.project.Directory, "postcorrection"),
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
	)
	if err != nil {
		return fmt.Errorf("cannot run %s: %v", script, err)
	}
	const stmnt = "UPDATE " + db.BooksTableName + " SET extendedlexicon=? WHERE bookid=?"
	_, err = db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot run %s: %v", script, err)
	}
	return nil
}

type pcRunner struct {
	pool    *sql.DB
	project *db.Project
}

func (r pcRunner) Name() string {
	return "calculating post-correction"
}

func (r pcRunner) BookID() int {
	return r.project.BookID
}

func (r pcRunner) Run(ctx context.Context) error {
	if err := r.setupWorkspace(); err != nil {
		return fmt.Errorf(
			"cannot calculate post correction: %v", err)
	}
	dir := filepath.Join(baseDir, r.project.Directory, "postcorrection")
	protocol := filepath.Join(dir, "dm-protocol.json")
	err := jobs.Run(
		ctx,
		"/apps/run.bash",
		"dm",
		dir,
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
	)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_rrdm.bash: %v", err)
	}
	if err := r.correct(protocol); err != nil {
		return fmt.Errorf("cannot correct: %v", err)
	}
	return nil
}

func (r pcRunner) setupWorkspace() error {
	doc, err := loadDocument(r.project)
	if err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	if err := doc.write(); err != nil {
		return fmt.Errorf("cannot setup workspace: %v", err)
	}
	return nil
}

func (r pcRunner) correct(path string) (err error) {
	var corrections api.PostCorrection
	if err := r.readProtocol(&corrections, path); err != nil {
		return fmt.Errorf("cannot read protocol %s: %v", path, err)
	}
	rnd := rand.New(rand.NewSource(time.Now().Unix()))
	// preprocess protocol
	for k, v := range corrections.Corrections {
		var bid, pid, lid, tid int
		if _, err := fmt.Sscanf(k, "%d:%d:%d:%d", &bid, &pid, &lid, &tid); err != nil {
			return fmt.Errorf("invalid protocol id: %s", k)
		}
		v.BookID = r.project.BookID
		v.ProjectID = r.project.ProjectID
		v.PageID = pid
		v.LineID = lid
		v.TokenID = tid
		if rnd.Int31n(10) < 3 { // 0, 1, 2
			v.Taken = true
		}
		corrections.Corrections[k] = v
	}

	if err := r.correctInBackend(&corrections); err != nil {
		return fmt.Errorf("cannot correct in backend: %v", err)
	}
	if err := r.correctInDatabase(&corrections); err != nil {
		return fmt.Errorf("cannot correct in database: %v", err)
	}
	if err := r.updateStatus(); err != nil {
		return fmt.Errorf("cannot update project status: %v", err)
	}
	if err := r.writeProtocol(&corrections, path); err != nil {
		return fmt.Errorf("cannot write protocol: %v", err)
	}
	return nil
}

func (r pcRunner) updateStatus() error {
	const stmnt = "UPDATE " + db.BooksTableName + " SET postcorrected=? WHERE bookid=?"
	_, err := db.Exec(r.pool, stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot execute database update: %v", err)
	}
	return nil
}

func (r pcRunner) deleteCorrections() error {
	const del = "DELETE FROM autocorrections WHERE bookid = ?"
	_, err := db.Exec(r.pool, del, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot delete old corrections from database: %v", err)
	}
	return nil
}

func (r pcRunner) readProtocol(pc *api.PostCorrection, path string) error {
	in, err := os.Open(path)
	if err != nil {
		return fmt.Errorf("cannot open protocol: %v", err)
	}
	defer in.Close()
	if err := json.NewDecoder(in).Decode(pc); err != nil {
		return fmt.Errorf("cannot decode protocol: %v", err)
	}
	return nil
}

func (r pcRunner) writeProtocol(pc *api.PostCorrection, path string) (err error) {
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

func (r pcRunner) correctInBackend(corrections *api.PostCorrection) error {
	// We should be communicating within docker compose - so skip verify is ok.
	client := api.NewClient(pocowebURL, true)
	for k, v := range corrections.Corrections {
		log.Debugf("correction: %s %v", k, v)
		if !v.Taken { // only correct corrections that should be taken
			continue
		}
		_, err := client.PutToken(v.BookID, v.PageID, v.LineID, v.TokenID,
			api.CorrectionRequest{Correction: v.Cor, Manually: false})
		if err != nil {
			return fmt.Errorf("cannot post correct %s: %v", v.Normalized, err)
		}
	}
	return nil
}

func (r pcRunner) correctInDatabase(corrections *api.PostCorrection) error {
	if err := r.deleteCorrections(); err != nil {
		return err
	}
	ins, err := r.pool.Prepare("INSERT INTO autocorrections" +
		"(bookid,pageid,lineid,tokenid,ocrtypid,cortypid,taken) " +
		"VALUES(?,?,?,?,?,?,?)")
	if err != nil {
		return fmt.Errorf("cannot prepare insert statement: %v", err)
	}
	defer ins.Close()
	t, err := db.NewTypeInserter(r.pool)
	if err != nil {
		return fmt.Errorf("cannot insert types: %v", err)
	}
	defer t.Close()

	// insert corrections
	for _, v := range corrections.Corrections {
		ocrtypid, err := t.ID(v.Normalized)
		if err != nil {
			return fmt.Errorf("cannot insert ocr type: %v", err)
		}
		cortypid, err := t.ID(v.Cor)
		if err != nil {
			return fmt.Errorf("cannot insert cor type: %v", err)
		}
		if _, err := ins.Exec(v.BookID, v.PageID, v.LineID, v.TokenID,
			ocrtypid, cortypid, v.Taken); err != nil {
			return fmt.Errorf("cannot insert correction: %v", err)
		}
	}
	return nil
}
