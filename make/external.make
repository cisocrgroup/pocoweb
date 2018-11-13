LIBS += lib/libsqlpp-mysql.a
LIBS += lib/libpugixml.a

lib/libsqlpp-mysql.a: ext/mysqlpp11/Makefile | mkdir-lib
	$(call ECHO,$@)
	$V cd ext/mysqlpp11 && $(MAKE) -j$(J)
	$V cp ext/mysqlpp11/src/libsqlpp-mysql.a $@

# for unkown reasons, cmake seems to automatically use LDFLAGS environment variable
# maybe this could be fixed using -DCMAKE_EXE_LINKER_FLAGS in the `cmake ...` call
ext/mysqlpp11/Makefile ext/mysqlpp11/CMakeCache.txt: $(MODS) | mkdir-ext-mysqlpp11
	$(call ECHO,$@)
	$V cd ext/mysqlpp11 && cmake -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_CXX_FLAGS=$CXXFLAGS -DCMAKE_CXX_FLAGS=-fpic ../../modules/sqlpp11-connector-mysql/
lib/libpugixml.a: modules/pugixml/build/pugixml.o | mkdir-lib
	$(call ECHO,$@)
	$V $(AR) rcs $@ $^
modules/pugixml/build/pugixml.o: modules/pugixml/src/pugixml.cpp | mkdir-modules-pugixml-build
	$(call ECHO,$@)
	$V $(CXX) $(patsubst -Wsuggest-override,,$(CXXFLAGS)) -c -o $@ $^
