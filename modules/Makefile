MODS += crow.xy
MODS += pugixml.xy
MODS += utfcpp.xy

all: $(MODS)
install:
uninstall:
clean:
	$(RM) *.xy

#.SECONDEXPANSION:
#%.xy: .git/modules/modules/$$*/HEAD
%.xy:
	touch $@
	git submodule update --init modules/$*

