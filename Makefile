CXX ?= g++

default: all

include make/config.mak
include make/modules.mak
include make/external.mak
include make/clean.mak
include make/rest.mak
include make/plugins.mak
include make/utils.mak
include make/test.mak
include make/frontend.mak

.PHONY: all default
-include $(DEPS)

mkdir-%:; @mkdir -p $(subst -,/,$*)
dbg-%:; @echo $*: ${$*}

all: ${ALL}

.PHONY: tags
tags:
	@echo "generating tags file"
	@ctags -R plugins rest modules
