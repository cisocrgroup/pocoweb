LIBS += lib/libsqlpp-mysql.a
LIBS += lib/libpugixml.a

lib/libsqlpp-mysql.a: ext/mysqlpp11/Makefile | mkdir-lib
	$(call ECHO,$@)
	$V cd ext/mysqlpp11 && $(MAKE)
	$V cp ext/mysqlpp11/src/libsqlpp-mysql.a $@

# for unkown reasons, cmake seems to automatically use LDFLAGS environment variable
# maybe this could be fixed using -DCMAKE_EXE_LINKER_FLAGS in the `cmake ...` call
ext/mysqlpp11/Makefile ext/mysqlpp11/CMakeCache.txt: $(MODS) | mkdir-ext-mysqlpp11
	$(call ECHO,$@)
	$V cd ext/mysqlpp11 && cmake -DDATE_INCLUDE_DIR=../../modules/date/include -DSQLPP11_INCLUDE_DIR=../../modules/sqlpp11/include/ -DCMAKE_EXE_LINKER_FLAGS="" -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_CXX_FLAGS=$(CXXFLAGS) -DCMAKE_CXX_FLAGS=-fpic ../../modules/sqlpp11-connector-mysql/

lib/libpugixml.a: ext/pugixml/pugixml.o | mkdir-lib
	$(call ECHO,$@)
	$V $(AR) rcs $@ $^

ext/pugixml/pugixml.o: modules/pugixml/src/pugixml.cpp | mkdir-ext-pugixml
	$(call ECHO,$@)
	$V $(CXX) $(patsubst -Wsuggest-override,,$(CXXFLAGS)) -c -o $@ $^

.PHONY: modules-clean
modules-clean:
	cd ext/mysqlpp11 && $(MAKE) clean

# Add https://github.com/azawadzki/base-n
CXXFLAGS += -isystem modules/base-n/include

# Add https://github.com/Tencent/rapidjson
CXXFLAGS += -isystem modules/rapidjson/include
