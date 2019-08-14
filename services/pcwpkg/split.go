package main

import (
	"database/sql"
	"fmt"
	"math/rand"
	"sort"

	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
)

type pkgs struct {
	BookID int   `json:"bookId"`
	Pkgs   []pkg `json:"projects"`
}

type pkg struct {
	ProjectID int   `json:"projectId"`
	Owner     int   `json:"owner"`
	PageIDs   []int `json:"pageIds"`
}

// len(r.UserIDs) must not be equal to 0
func split(bookID int, r splitProjectRequest) (*pkgs, error) {
	if len(r.UserIDs) == 0 {
		panic("invalid userids")
	}
	pageIDs, err := loadPageIDs(bookID)
	if err != nil {
		return nil, fmt.Errorf("cannot split project: %v", err)
	}
	dist := distribute(pageIDs, r)
	if err := insert(bookID, dist); err != nil {
		return nil, fmt.Errorf("cannot split project: %v", err)
	}
	return &pkgs{
		BookID: bookID,
		Pkgs:   dist,
	}, nil
}

func insert(bookID int, pkgs []pkg) error {
	// prepare insert statements
	tx, err := service.Pool().Begin()
	if err != nil {
		return fmt.Errorf("cannot insert pkgs: %v", err)
	}
	insertp, err := tx.Prepare(
		`insert into projects (origin,owner,pages) values(?,?,?)`,
	)
	if err != nil {
		return fmt.Errorf("cannot insert pkgs: %v", err)
	}
	defer insertp.Close()
	insertpp, err := tx.Prepare(
		`insert into project_pages (projectid,pageid,prevpageid,nextpageid) values(?,?,?,?)`,
	)
	if err != nil {
		return fmt.Errorf("cannot insert pkgs: %v", err)
	}
	defer insertpp.Close()
	// insert packages
	for i := range pkgs {
		res, err := insertp.Exec(bookID, pkgs[i].Owner, len(pkgs[i].PageIDs))
		if err != nil {
			tx.Rollback()
			return fmt.Errorf("cannot insert pkgs: %v", err)
		}
		packageID, err := res.LastInsertId()
		if err != nil {
			tx.Rollback()
			return fmt.Errorf("cannot insert pkg %d: %v", packageID, err)
		}
		pkgs[i].ProjectID = int(packageID)
		if err := insertPageIDs(tx, insertpp, pkgs[i], packageID); err != nil {
			tx.Rollback()
			return fmt.Errorf("cannot insert pkg %d: %v", packageID, err)
		}
	}
	if err := tx.Commit(); err != nil {
		return fmt.Errorf("cannot insert pkgs: %v", err)
	}
	return nil
}

func insertPageIDs(tx *sql.Tx, ins *sql.Stmt, p pkg, id int64) error {
	for i, pageID := range p.PageIDs {
		prev := pageID
		next := pageID
		if i > 0 {
			prev = p.PageIDs[i-1]
		}
		if i+1 < len(p.PageIDs) {
			next = p.PageIDs[i+1]
		}
		if _, err := ins.Exec(id, pageID, prev, next); err != nil {
			tx.Rollback()
			return fmt.Errorf("cannot insert page %d: %v", pageID, err)
		}
	}
	return nil
}

func distribute(pageIDs []int, r splitProjectRequest) []pkg {
	if r.Random {
		rand.Shuffle(len(pageIDs), func(i, j int) {
			pageIDs[i], pageIDs[j] = pageIDs[j], pageIDs[i]
		})
	}
	n := len(r.UserIDs)
	d := len(pageIDs) / n
	x := len(pageIDs) % n
	o := 0
	ret := make([]pkg, n)
	// assign page ids in order (assuming that page ids are in order)
	// first packages get one more page until x <= 0
	for i := 0; i < n; i++ {
		m := d
		if x > 0 {
			m += 1
			x--
		}
		ret[i].Owner = r.UserIDs[i]
		for j := 0; j < m; j++ {
			ret[i].PageIDs = append(ret[i].PageIDs, pageIDs[j+o])
		}
		o += m
		// sort page ids if original page ids where randomized
		if r.Random {
			sort.Slice(ret[i].PageIDs, func(a, b int) bool {
				return ret[i].PageIDs[a] < ret[i].PageIDs[b]
			})
		}
	}
	return ret
}

func loadPageIDs(bookID int) ([]int, error) {
	const stmt = `select pageid from project_pages where projectid = ? order by pageid asc`
	rows, err := db.Query(service.Pool(), stmt, bookID)
	if err != nil {
		return nil, fmt.Errorf("cannot query page ids: %v", err)
	}
	defer rows.Close()
	var pageIDs []int
	for rows.Next() {
		var pageID int
		if err := rows.Scan(&pageID); err != nil {
			return nil, fmt.Errorf("cannot scan page id: %v", err)
		}
		pageIDs = append(pageIDs, pageID)
	}
	return pageIDs, nil
}
