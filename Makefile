CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
SHELL = /bin/bash

export $(CXX)
export $(CXXFLAGS)
export $(SHELL)
export $(LDFLAGS)

all: default

default install clean uninstall: config.mk
	$(MAKE) -C rest $@
	$(MAKE) -C db $@

config.mk: misc/default/config.def.mk
	cp $< $@
