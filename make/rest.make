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
CORE_OBJS += rest/src/core/Page.o
CORE_OBJS += rest/src/core/PageBuilder.o
CORE_OBJS += rest/src/core/Project.o
CORE_OBJS += rest/src/core/ProjectBuilder.o
CORE_OBJS += rest/src/core/Route.o
CORE_OBJS += rest/src/core/Searcher.o
CORE_OBJS += rest/src/core/Session.o
CORE_OBJS += rest/src/core/SessionDirectory.o
CORE_OBJS += rest/src/core/SessionStore.o
CORE_OBJS += rest/src/core/WagnerFischer.o
CORE_OBJS += rest/src/core/XmlFile.o
CORE_OBJS += rest/src/core/jsonify.o
CORE_OBJS += rest/src/core/queries.o
CORE_OBJS += rest/src/core/util.o
CORE_OBJS += rest/src/database/DbStructs.o

API_OBJS += rest/src/api/BookRoute.o
API_OBJS += rest/src/api/CharMapRoute.o
API_OBJS += rest/src/api/DownloadRoute.o
API_OBJS += rest/src/api/LineRoute.o
API_OBJS += rest/src/api/PageRoute.o
API_OBJS += rest/src/api/SearchRoute.o
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
PARSER_OBJS += rest/src/parser/PageXmlPageParser.o
PARSER_OBJS += rest/src/parser/PageXmlParserLine.o
PARSER_OBJS += rest/src/parser/ParserPage.o
PARSER_OBJS += rest/src/parser/Xml.o
PARSER_OBJS += rest/src/parser/XmlParserPage.o
PARSER_OBJS += rest/src/parser/hocr.o
PARSER_OBJS += rest/src/parser/llocs.o

LIBS += lib/libpcw.a
MAINS += pcwd

lib/libpcw.a: $(CORE_OBJS) $(API_OBJS) $(PARSER_OBJS) | $(MODS) mkdir-lib
	$(call ECHO,$@)
	$V $(AR) rcs $@ $^
pcwd: rest/src/pcwd.o $(LIBS)
	$(call ECHO,$@)
	$V $(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@
rest/src/database/Tables.h: db/tables.sql.tmp
	$(call ECHO,$@)
	$V modules/sqlpp11/scripts/ddl2cpp $^ rest/src/database/Tables tables
# ddl2cpp does not handle `create table if not exists foo`
db/tables.sql.tmp: db/tables.sql
	$(call ECHO,$@)
	@sed -e '/create/ s/if\s\s*not\s\s*exists\s*//' \
		 -e '/character set/d' \
		 -e '/insert/d' \
         -e '/values/d' \
		 -e '/--/d' $< > $@


install: pcwd
	$(call install pcwd)
	$V install -d $(BINDIR)
	$V install -m 751 $< $(BINDIR)/pcwd

DEPS += $(patsubst %.o,%.d,$(CORE_OBJS) $(API_OBJS) $(PARSER_OBJS) $(PROFILER_OBJS) $(PUGI_OBJS))
DEPS += $(patsubst %,%.d,$(MAINS))
ALL += $(LIBS) $(MAINS)
