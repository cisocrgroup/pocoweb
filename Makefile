CXXFLAGS ?= -Wall -Werror -std=gnu++14 -g -Og
CXXFLAGS := $(CXXFLAGS) -MD -MP
SHELL ?= /bin/bash

PCW_API_VERSION_MAJOR = 0
PCW_API_VERSION_MINOR = 1
PCW_API_VERSION_PATCH = 0

## export $(CXX)
## export $(CXXFLAGS)
## export $(SHELL)
## export $(LDFLAGS)
## export $(PCW_API_VERSION)

export # export all

default: all

all install clean uninstall:
	$(MAKE) -C modules $@
	#$(MAKE) -C misc $@
	$(MAKE) -C rest $@
	$(MAKE) -C plugins $@
	#$(MAKE) -C db $@ SHELL=$(SHELL)

