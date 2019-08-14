package main

import (
	"fmt"

	"github.com/finkf/pcwgo/db"
	"github.com/finkf/pcwgo/service"
)

func assignToOriginalOwner(bookID int) error {
	orig, err := loadOriginalOwner(bookID)
	if err != nil {
		return fmt.Errorf("cannot assign back to original user: %v", err)
	}
	if err := assignToUser(bookID, orig); err != nil {
		return fmt.Errorf("cannot assign back to original user: %v", err)
	}
	return nil
}

func loadOriginalOwner(bookID int) (int, error) {
	stmt := `select b.owner from projects a ` +
		`join projects b on a.origin=b.id ` +
		`where a.id=?`
	rows, err := db.Query(service.Pool(), stmt, bookID)
	if err != nil {
		return 0, fmt.Errorf("cannot query original user id: %v", err)
	}
	defer rows.Close()
	if !rows.Next() {
		return 0, fmt.Errorf("cannot query original user id: not found")
	}
	var ret int
	if err := rows.Scan(&ret); err != nil {
		return 0, fmt.Errorf("cannot query original user id: %v", err)
	}
	return ret, nil
}

func assignToUser(bookID, userID int) error {
	stmt := `update projects set owner=? where id=?`
	if _, err := db.Exec(service.Pool(), stmt, userID, bookID); err != nil {
		return fmt.Errorf("cannot assign project to user: %v", err)
	}
	return nil
}

func takeBack(bookID, userID int) error {
	stmt := `update projects set owner=? where origin=?`
	if _, err := db.Exec(service.Pool(), stmt, userID, bookID); err != nil {
		return fmt.Errorf("cannot take back project: %v", err)
	}
	return nil
}
