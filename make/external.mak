LIBS += lib/libsqlpp-mysql.a
LIBS += lib/libpugixml.a

lib/libsqlpp-mysql.a: ext/mysqlpp11/Makefile | mkdir-lib
	@cd ext/mysqlpp11 && make -j$(J)
	@cp ext/mysqlpp11/src/libsqlpp-mysql.a $@
ext/mysqlpp11/Makefile: $(MODS) | mkdir-ext-mysqlpp11
	@cd ext/mysqlpp11 && cmake -DCMAKE_CXX_FLAGS=-fpic ../../modules/sqlpp11-connector-mysql
lib/libpugixml.a: modules/pugixml/build/pugixml.o | mkdir-lib
	$(AR) rcs $@ $^
modules/pugixml/build/pugixml.o: modules/pugixml/src/pugixml.cpp | mkdir-modules-pugixml-build
	$(CXX) $(CXXFLAGS) -c -o $@ $^
