CORE_OBJS += rest/src/core/WagnerFischer.o
CORE_OBJS += rest/src/core/MetsXmlBookParser.o
CORE_OBJS += rest/src/core/BookDirectoryBuilder.o
CORE_OBJS += rest/src/core/BookConstructor.o
CORE_OBJS += rest/src/core/BookBuilder.o
CORE_OBJS += rest/src/core/LineBuilder.o
CORE_OBJS += rest/src/core/BookFixer.o
CORE_OBJS += rest/src/core/Box.o
CORE_OBJS += rest/src/core/Book.o
CORE_OBJS += rest/src/core/Corrector.o
CORE_OBJS += rest/src/core/BookView.o
CORE_OBJS += rest/src/core/Package.o
CORE_OBJS += rest/src/core/Config.o
CORE_OBJS += rest/src/core/Database.o
CORE_OBJS += rest/src/core/NewDatabase.o
CORE_OBJS += rest/src/core/Profile.o
CORE_OBJS += rest/src/core/Password.o
CORE_OBJS += rest/src/core/Route.o
CORE_OBJS += rest/src/core/App.o
CORE_OBJS += rest/src/core/Line.o
CORE_OBJS += rest/src/core/User.o
CORE_OBJS += rest/src/core/Sessions.o
CORE_OBJS += rest/src/core/XmlFile.o
CORE_OBJS += rest/src/core/util.o
CORE_OBJS += rest/src/core/jsonify.o
CORE_OBJS += rest/src/core/docxml.o
CORE_OBJS += rest/src/core/db.o

API_OBJS += rest/src/api/VersionRoute.o
API_OBJS += rest/src/api/BookRoute.o
API_OBJS += rest/src/api/PageRoute.o
API_OBJS += rest/src/api/LineRoute.o

PARSER_OBJS += rest/src/parser/ParserPage.o
PARSER_OBJS += rest/src/parser/Xml.o
PARSER_OBJS += rest/src/parser/XmlParserPage.o
PARSER_OBJS += rest/src/parser/AltoXmlParserLine.o
PARSER_OBJS += rest/src/parser/AltoXmlPageParser.o
PARSER_OBJS += rest/src/parser/AbbyyXmlParserLine.o
PARSER_OBJS += rest/src/parser/AbbyyXmlPageParser.o
PARSER_OBJS += rest/src/parser/OcropusLlocsParserLine.o
PARSER_OBJS += rest/src/parser/OcropusLlocsPageParser.o
PARSER_OBJS += rest/src/parser/OcropusLlocsParserPage.o
PARSER_OBJS += rest/src/parser/hocr.o
PARSER_OBJS += rest/src/parser/HocrPageParser.o
PARSER_OBJS += rest/src/parser/HocrParserLine.o

PUGI_OBJS += rest/src/pugixml/pugixml.o

LIBS += libpcwcore.a
LIBS += libpcwparser.a
LIBS += libpcwapi.a
LIBS += libpcwpugi.a
MAINS += pcwd
MAINS += pcwc

libpcwcore.a: $(CORE_OBJS) $(VENDOR_MAKEFILES)
	$(AR) rcs $@ $^
libpcwapi.a: $(API_OBJS) $(VENDOR_MAKEFILES)
	$(AR) rcs $@ $^
libpcwparser.a: $(PARSER_OBJS) $(VENDOR_MAKEFILES)
	$(AR) rcs $@ $^
libpcwpugi.a: $(PUGI_OBJS) $(VENDOR_MAKEFILES)
	$(AR) rcs $@ $^

pcwd: rest/src/pcwd.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
pcwc: rest/src/pcwc.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -lcurl

rest/src/core/Tables.hpp: modules/sqlpp11/scripts/ddl2cpp db/tables.sql
	$^ rest/src/core/Tables tables

DEPS += $(patsubst %.o,%.d,$(CORE_OBJS) $(API_OBJS) $(PARSER_OBJS) $(PUGI_OBJS))
DEPS += $(patsubst %,%.d,$(MAINS))
ALL += $(LIBS) $(MAINS)
