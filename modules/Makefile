MODS += crow.mod
MODS += pugixml.mod
MODS += utfcpp.mod

all: $(MODS)
install:
uninstall:
clean:
	$(RM) *.mod

.SECONDEXPANSION:
%.mod: ../.git/modules/modules/$$*/HEAD
	git submodule update --init $*
	touch $@

