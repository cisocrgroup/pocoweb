MODS += modules/crow.gitmodule
MODS += modules/pugixml.gitmodule
MODS += modules/utfcpp.gitmodule
MODS += modules/sqlpp11.gitmodule
MODS += modules/sqlpp11-connector-mysql.gitmodule
MODS += modules/date.gitmodule

.DELETE_ON_ERROR:
%.gitmodule:
	$(call ECHO,$@)
	$V flock mod.lock git submodule update --init $*
	$V touch $@
	$V $(RM) -f mod.lock

ALL += $(MODS)
