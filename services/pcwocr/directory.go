package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

type ocrDirectory struct {
	base string
}

func newOCRDirectory(base, name string) (ocrDirectory, error) {
	dir := ocrDirectory{base: filepath.Join(base, name)}
	if err := os.MkdirAll(dir.base, 0755); err != nil {
		return ocrDirectory{}, fmt.Errorf("cannot create ocr directory %s: %v",
			dir.base, err)
	}
	return dir, nil
}

// Close deletes the base directory.
func (dir ocrDirectory) Close() error {
	return os.RemoveAll(dir.base)
}

func (dir ocrDirectory) addFilePath(src string, bookID int) error {
	name := filepath.Base(src)
	destDir := filepath.Join(dir.base, fmt.Sprintf("%04d", bookID))
	dest := filepath.Join(destDir, name)
	log.Printf("adding file %s to %s", src, dest)
	if err := os.MkdirAll(destDir, 0755); err != nil {
		return fmt.Errorf("cannot create directory %s: %v", destDir, err)
	}
	return linkOrCopy(src, dest)
}

func (dir ocrDirectory) addFileContent(src, cont string, bookID int) (err error) {
	name := filepath.Base(src)
	destDir := filepath.Join(dir.base, fmt.Sprintf("%04d", bookID))
	dest := filepath.Join(destDir, name)
	return ioutil.WriteFile(dest, []byte(cont), 0644)
}

// hard-links or copies (if hard-linking fails) a file.
func linkOrCopy(src, dest string) (err error) {
	err = os.Link(src, dest)
	if err == nil || os.IsExist(err) { // we have created the link and are done
		return nil
	}
	// copy the file the hard way
	log.Printf("cannot hard-link file %s to %s; copying", src, dest)
	in, err := os.Open(src)
	if err != nil {
		return fmt.Errorf("cannot copy file %s to %s: %v", src, dest, err)
	}
	defer in.Close()
	out, err := os.Create(dest)
	if err != nil {
		return fmt.Errorf("cannot copy file %s to %s: %v", src, dest, err)
	}
	defer func() { // handle errors on close for destination file
		cerr := out.Close()
		if err != nil {
			err = cerr
		}
	}()
	if _, err = io.Copy(out, in); err != nil {
		return fmt.Errorf("cannot copy file %s to %s: %v", src, dest, err)
	}
	return out.Sync()
}
