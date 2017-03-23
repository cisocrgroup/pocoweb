default: all

include make/config.mak
include make/modules.mak
include make/external.mak
include make/clean.mak
include make/rest.mak
include make/plugins.mak
include make/test.mak

.PHONY: all default
-include $(DEPS)

mkdir-%:; @mkdir -p $(subst -,/,$*)
dbg-%:; @echo $*: ${$*}

all: ${ALL}

## tags: $(SRCS)
## 	@echo "generating tags file"
## 	@ctags $(SRCS)
