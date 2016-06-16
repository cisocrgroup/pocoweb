CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
SHELL ?= /bin/bash

PCW_API_VERSION_MAJOR = 0
PCW_API_VERSION_MINOR = 1
PCW_API_VERSION_PATCH = 0
PCW_API_VERSION = "$(PCW_API_VERSION_MAJOR).$(PCW_API_VERSION_MINOR).$(PCW_API_VERSION_PATCH)"

export $(CXX)
export $(CXXFLAGS)
export $(SHELL)
export $(LDFLAGS)
export $(PCW_API_VERSION)

all: default

default install clean uninstall: config.mk
	$(MAKE) -C rest $@ SHELL=$(SHELL) PCW_API_VERSION=$(PCW_API_VERSION)
#	$(MAKE) -C db $@ SHELL=$(SHELL)

config.mk: misc/default/config.def.mk
	cp $< $@
