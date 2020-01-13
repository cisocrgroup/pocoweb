package main

import (
	"encoding/json"
	"fmt"
	"os"
)

type ExtendedData struct {
	LinePath    string       `json:"linePath"`
	Predictions []Prediction `json:"predictions"`
}

type Prediction struct {
	TotalProbability       float32    `json:"totalProbability"`
	AverageCharProbability float32    `json:"avgCharProbability"`
	Labels                 []int      `json:"labels"`
	IsVotedResult          bool       `json:"isVotedResult"`
	LinePath               string     `json:"linePath"`
	ID                     string     `json:"id"`
	Sentence               string     `json:"sentence"`
	Positions              []Position `json:"positions"`
	Logits                 Logits     `json:"logits"`
}

type Position struct {
	GlobalStart int    `json:"globalStart"`
	GlobalEnd   int    `json:"globalEnd"`
	LocalStart  int    `json:"localStart"`
	LocalEnd    int    `json:"localEnd"`
	Chars       []Char `json:"chars"`
}

type Char struct {
	Probability float32 `json:"probability"`
	Char        string  `json:"char"`
	Label       int     `json:"label"`
}

type Logits struct {
	Cols int   `json:"cols"`
	Rows int   `json:"rows"`
	Data []int `json:"data"`
}

func readExtendedDataFromJSON(p string) (*ExtendedData, error) {
	in, err := os.Open(p)
	if err != nil {
		return nil, fmt.Errorf("cannot read extended data: %v", err)
	}
	defer in.Close()
	var data ExtendedData
	if err := json.NewDecoder(in).Decode(&data); err != nil {
		return nil, fmt.Errorf("cannot read extended data: %v", err)
	}
	return &data, nil
}
