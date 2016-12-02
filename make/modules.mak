MODS += modules/crow.gitmodule
MODS += modules/pugixml.gitmodule
MODS += modules/utfcpp.gitmodule

%.gitmodule:
	git submodule update --init $*
	touch $@

ALL += $(MODS)
