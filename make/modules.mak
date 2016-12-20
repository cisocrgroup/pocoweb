MODS += modules/crow.gitmodule
MODS += modules/pugixml.gitmodule
MODS += modules/utfcpp.gitmodule

%.gitmodule:
	flock mod.lock git submodule update --init $*
	touch $@

ALL += $(MODS)
