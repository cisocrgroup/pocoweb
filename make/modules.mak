MODS += modules/crow.gitmodule
MODS += modules/pugixml.gitmodule
MODS += modules/utfcpp.gitmodule
MODS += modules/sqlpp11.gitmodule
MODS += modules/sqlpp11-connector-mysql.gitmodule
MODS += modules/date.gitmodule

.DELETE_ON_ERROR:
%.gitmodule:
	git submodule update --init $*
	touch $@

ALL += $(MODS)
