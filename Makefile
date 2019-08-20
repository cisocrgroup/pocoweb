CXX ?= g++

default: all

include make/config.make
include make/external.make
include make/clean.make
include make/rest.make
include make/test.make
include make/docker.make

.PHONY: all default
-include $(DEPS)

all: ${ALL}
