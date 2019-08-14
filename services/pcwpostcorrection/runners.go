package main

import (
	"context"
	"fmt"
	"path/filepath"

	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/jobs"
	"github.com/finkf/pcwgo/service"
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
	err := jobs.Run(
		ctx,
		"/apps/run_rrdm.bash",
		config,
		filepath.Join(baseDir, r.project.Directory, "profile.json.gz"),
		filepath.Join(baseDir, r.project.Directory, "postcorrection", "rrdm.json"),
	)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_rrdm.bash: %v", err)
	}
	const stmnt = "UPDATE " + db.BooksTableName + " SET postcorrected=? WHERE bookid=?"
	_, err = db.Exec(service.Pool(), stmnt, true, r.project.BookID)
	if err != nil {
		return fmt.Errorf("cannot run /apps/run_rrdm.bash: %v", err)
	}
	service.UncacheProject(r.project)
	return nil
}
