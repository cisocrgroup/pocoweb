package main

import (
	"os"
	"path/filepath"
	"testing"
)

func TestAddFilePath(t *testing.T) {
	tests := []struct {
		src, base, name string
		bookID          int
		wantErr         bool
	}{
		{"testdata/0001/00001.bin.png", "testdata", "out", 1, false},
		{"testdata/0042/00148.bin.png", "testadata", "out", 42, false},
		{"testdata/0001/00001.bin.png", "/tmp", "pcwocr-test", 1, false},
		{"testdata/0042/00148.bin.png", "/tmp", "pcwocr-test", 42, false},
		{"testdata/0008/00148.bin.png", "/tmp", "pcwocr-test", 42, true},
	}
	for _, tc := range tests {
		t.Run(tc.src, func(t *testing.T) {
			dir, err := newOCRDirectory(tc.base, tc.name)
			want := filepath.Join(tc.base, tc.name, tc.src[9:])
			if err != nil {
				t.Fatalf("got error: %v", err)
			}
			err = dir.addFilePath(tc.src, tc.bookID)
			if tc.wantErr && err == nil {
				t.Fatalf("expected an error")
			}
			if !tc.wantErr && err != nil {
				t.Fatalf("got error: %v", err)
			}
			if _, err = os.Stat(want); tc.wantErr != os.IsNotExist(err) {
				t.Fatalf("%s exists but shouldn't or doesn't exist but should",
					want)
			}
			if err := dir.Close(); err != nil {
				t.Fatalf("got error: %v", err)
			}
			if _, err = os.Stat(dir.base); !os.IsNotExist(err) {
				t.Fatalf("directory close did not delete the directory")
			}
		})
	}
}
