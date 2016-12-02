ALL :=
DEPS :=
default: all

include make/config.mak
include make/modules.mak
include make/clean.mak
include make/rest.mak
include make/plugins.mak
include make/test.mak

all: $(ALL)
.PHONY: all default
-include $(DEPS)

## tags: $(SRCS)
## 	@echo "generating tags file"
## 	@ctags $(SRCS)
