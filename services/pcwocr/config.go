package main

import "github.com/BurntSushi/toml"

type configuration struct {
	TrainArgs   []string
	PredictArgs []string
}

func loadConfig(p string) error {
	_, err := toml.DecodeFile(p, &config)
	return err
}
