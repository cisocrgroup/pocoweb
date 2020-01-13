package main

import (
	"bufio"
	"context"
	"fmt"
	"io"
	"log"
	"os/exec"
	"sync"
)

type ocropy struct {
	model string
	out   io.Writer
	ctx   context.Context
	// private
	// wg    *sync.WaitGroup
	// mutex *sync.Mutex
	wg    sync.WaitGroup
	mutex sync.Mutex
}

func (o *ocropy) run(cmd string, args ...string) error {
	if o.model != "" {
		args = append([]string{"-m", o.model}, args...)
	}
	return o.doRun(cmd, args...)
}

func (o *ocropy) doRun(cmd string, args ...string) error {
	ctx, cancel := context.WithCancel(o.ctx)
	// ctx := o.ctx
	exe := exec.CommandContext(ctx, cmd, args...)
	stderr, err := exe.StderrPipe()
	if err != nil {
		cancel() // vet
		return fmt.Errorf("cannot run: %v", err)
	}
	stdout, err := exe.StdoutPipe()
	if err != nil {
		cancel() // vet
		return fmt.Errorf("cannot run: %v", err)
	}
	// o.wg = &sync.WaitGroup{}
	// o.mutex = &sync.Mutex{}
	o.wg.Add(2)
	go o.read(2, stderr, cancel)
	go o.read(1, stdout, cancel)
	log.Printf("running: %s %v", cmd, args)
	if err := exe.Start(); err != nil {
		return fmt.Errorf("cannot run: %v", err)
	}
	o.wg.Wait()
	if err := exe.Wait(); err != nil {
		return fmt.Errorf("cannot run: %v", err)
	}
	return nil
}

func (o *ocropy) read(i int, in io.ReadCloser, cancel context.CancelFunc) {
	defer o.wg.Done()
	defer in.Close()
	if o.out == nil {
		return
	}
	s := bufio.NewScanner(in)
	for s.Scan() {
		func() { // defer is function scoped
			o.mutex.Lock()
			defer o.mutex.Unlock()
			if o.done() {
				return
			}
			bytes := s.Bytes()
			bytes = append(bytes, '\n')
			// log.Printf("got fucking lock")
			// if o.done() {
			// 	return
			// }
			if _, err := o.out.Write(bytes); err != nil {
				cancel()
				return
			}
		}()
	}
	if s.Err() != nil {
		cancel()
	}
}

func (o *ocropy) done() bool {
	select {
	case <-o.ctx.Done():
		log.Printf("we are fucking done")
		return true
	default:
		return false
	}
}
