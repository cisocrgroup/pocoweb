MODS += modules/crow.mod
MODS += modules/pugixml.mod
MODS += modules/utfcpp.mod

%.mod:
	git submodule update --init $*
	touch $@

