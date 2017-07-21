CORE_OBJS += rest/src/core/App.o
CORE_OBJS += rest/src/core/Archiver.o
CORE_OBJS += rest/src/core/Book.o
CORE_OBJS += rest/src/core/BookBuilder.o
CORE_OBJS += rest/src/core/BookConstructor.o
CORE_OBJS += rest/src/core/BookDirectoryBuilder.o
CORE_OBJS += rest/src/core/BookFixer.o
CORE_OBJS += rest/src/core/Box.o
CORE_OBJS += rest/src/core/Config.o
CORE_OBJS += rest/src/core/Corrector.o
CORE_OBJS += rest/src/core/Line.o
CORE_OBJS += rest/src/core/LineBuilder.o
CORE_OBJS += rest/src/core/MetsXmlBookParser.o
CORE_OBJS += rest/src/core/Package.o
CORE_OBJS += rest/src/core/PackageBuilder.o
CORE_OBJS += rest/src/core/PageBuilder.o
CORE_OBJS += rest/src/core/Password.o
CORE_OBJS += rest/src/core/Project.o
CORE_OBJS += rest/src/core/ProjectBuilder.o
CORE_OBJS += rest/src/core/Route.o
CORE_OBJS += rest/src/core/Searcher.o
CORE_OBJS += rest/src/core/Session.o
CORE_OBJS += rest/src/core/SessionDirectory.o
CORE_OBJS += rest/src/core/SessionStore.o
CORE_OBJS += rest/src/core/User.o
CORE_OBJS += rest/src/core/WagnerFischer.o
CORE_OBJS += rest/src/core/XmlFile.o
CORE_OBJS += rest/src/core/jsonify.o
CORE_OBJS += rest/src/core/util.o

API_OBJS += rest/src/api/BookRoute.o
API_OBJS += rest/src/api/CorrectionRoute.o
API_OBJS += rest/src/api/LineRoute.o
API_OBJS += rest/src/api/PageRoute.o
API_OBJS += rest/src/api/ProfilerRoute.o
API_OBJS += rest/src/api/UserRoute.o
API_OBJS += rest/src/api/VersionRoute.o

PARSER_OBJS += rest/src/parser/AbbyyXmlPageParser.o
PARSER_OBJS += rest/src/parser/AbbyyXmlParserLine.o
PARSER_OBJS += rest/src/parser/AltoXmlPageParser.o
PARSER_OBJS += rest/src/parser/AltoXmlParserLine.o
PARSER_OBJS += rest/src/parser/HocrPageParser.o
PARSER_OBJS += rest/src/parser/HocrParserLine.o
PARSER_OBJS += rest/src/parser/OcropusLlocsPageParser.o
PARSER_OBJS += rest/src/parser/OcropusLlocsParserLine.o
PARSER_OBJS += rest/src/parser/OcropusLlocsParserPage.o
PARSER_OBJS += rest/src/parser/ParserPage.o
PARSER_OBJS += rest/src/parser/Xml.o
PARSER_OBJS += rest/src/parser/XmlParserPage.o
PARSER_OBJS += rest/src/parser/hocr.o
PARSER_OBJS += rest/src/parser/llocs.o

PROFILER_OBJS += rest/src/profiler/LocalProfiler.o
PROFILER_OBJS += rest/src/profiler/Profile.o
PROFILER_OBJS += rest/src/profiler/Profiler.o
PROFILER_OBJS += rest/src/profiler/RemoteProfiler.o
PROFILER_OBJS += rest/src/profiler/docxml.o

LIBS += lib/libpcwcore.a
LIBS += lib/libpcwparser.a
LIBS += lib/libpcwprofiler.a
LIBS += lib/libpcwapi.a
MAINS += pcwd

lib/libpcwcore.a: $(CORE_OBJS) | $(MODS) mkdir-lib
	$(AR) rcs $@ $^
lib/libpcwapi.a: $(API_OBJS) | $(MODS) $(VENDS) mkdir-lib
	$(AR) rcs $@ $^
lib/libpcwparser.a: $(PARSER_OBJS) | $(MODS) $(VENDS) mkdir-lib
	$(AR) rcs $@ $^
lib/libpcwprofiler.a: $(PROFILER_OBJS) | $(MODS) $(VENDS) mkdir-lib
	$(AR) rcs $@ $^

pcwd: rest/src/pcwd.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

rest/src/database/Tables.h: modules/sqlpp11/scripts/ddl2cpp db/tables.sql
	$^ rest/src/database/Tables tables

DEPS += $(patsubst %.o,%.d,$(CORE_OBJS) $(API_OBJS) $(PARSER_OBJS) $(PROFILER_OBJS) $(PUGI_OBJS))
DEPS += $(patsubst %,%.d,$(MAINS))
ALL += $(LIBS) $(MAINS)
