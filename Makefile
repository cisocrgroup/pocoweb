CXXFLAGS ?= -Wall -Werror -std=gnu++14 -ggdb -Og
CXXFLAGS := $(CXXFLAGS) -MD -MP
SHELL ?= /bin/bash

PCW_API_VERSION_MAJOR = 0
PCW_API_VERSION_MINOR = 1
PCW_API_VERSION_PATCH = 0

SRCS=$(shell find . -type f -regex '.*\.[hc]p?p?$$')

## export $(CXX)
## export $(CXXFLAGS)
## export $(SHELL)
## export $(LDFLAGS)
## export $(PCW_API_VERSION)

export # export all

default: all tags

.PHONY: all install clean uninstall test
all install uninstall test:
	$(MAKE) -C modules $@
	#$(MAKE) -C misc $@
	$(MAKE) -C rest $@
	$(MAKE) -C plugins $@
	#$(MAKE) -C db $@ SHELL=$(SHELL)

clean:
	$(MAKE) -C modules $@
	#$(MAKE) -C misc $@
	$(MAKE) -C rest $@
	$(MAKE) -C plugins $@
	#$(MAKE) -C db $@ SHELL=$(SHELL)
	$(RM) tags

tags: $(SRCS)
	@echo "generating tags file"
	@ctags $(SRCS)

