CXX ?= g++

default: all

include make/config.make
include make/modules.make
include make/external.make
include make/clean.make
include make/rest.make
include make/plugins.make
include make/test.make
include make/frontend.make

.PHONY: all default
-include $(DEPS)

all: ${ALL}

.PHONY: tags
tags:
	@echo [ $@ ]
    ##@echo "generating tags file"
	@ctags -R plugins rest modules utils
