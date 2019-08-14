package main

import "github.com/finkf/pcwgo/api"

type Model struct {
	api.Model
	path string
}

type RecognizeModel struct {
	Name  string `json:"name"`
	Epoch int    `json:"epoch"`
}

type RetrainModel struct {
	RecognizeModel
	NTrain int `json:"ntrain"`
}
