CXXFLAGS := $(CXXFLAGS)
CXXFLAGS += -fpic
CXXFLAGS += -I src
CXXFLAGS += -I ../modules/crow/include
CXXFLAGS += -I ../modules/pugixml/src
CXXFLAGS += -I ../modules/utfcpp/source
CXXFLAGS += -DPCW_API_VERSION_MAJOR=$(PCW_API_VERSION_MAJOR)
CXXFLAGS += -DPCW_API_VERSION_MINOR=$(PCW_API_VERSION_MINOR)
CXXFLAGS += -DPCW_API_VERSION_PATCH=$(PCW_API_VERSION_PATCH)
CXXFLAGS += -DBOOST_LOG_DYN_LINK

LDFLAGS ?=
LDFLAGS := $(LDFLAGS)
LDFLAGS += -L.
LDFLAGS += -lpcwapi
LDFLAGS += -lpcwcore
LDFLAGS += -lpcwparser
LDFLAGS += -lpcwcore
LDFLAGS += -lpcwparser
LDFLAGS += -ldl
LDFLAGS += -lmysqlcppconn
LDFLAGS += -lssl
LDFLAGS += -llept
LDFLAGS += -lpthread
LDFLAGS += -lboost_log
LDFLAGS += -licuuc
# LDFLAGS += -lboost_regex
LDFLAGS += -lboost_system
# LDFLAGS += -lboost_coroutine
LDFLAGS += -lboost_filesystem
# LDFLAGS += -lboost_log
# LDFLAGS += -lboost_log_setup
# LDFLAGS += -lboost_thread
LDFLAGS += -lcrypto

VPATH = src src/core ../modules/pugixml/src src/api src/parser src/tests

OBJS += HocrPageParser.o
OBJS += WagnerFischer.o
OBJS += LlocsPageParser.o
OBJS += MetsXmlBookParser.o
OBJS += BookDirectoryBuilder.o
OBJS += BookBuilder.o
OBJS += BookFixer.o
OBJS += Box.o
OBJS += Book.o
OBJS += Project.o
OBJS += SubProject.o
OBJS += Config.o
OBJS += Database.o
OBJS += Route.o
OBJS += App.o
OBJS += Line.o
OBJS += User.o
OBJS += Sessions.o
OBJS += XmlFile.o
OBJS += util.o
OBJS += jsonify.o
OBJS += db.o

API_OBJS += ApiVersion.o
API_OBJS += ApiBooks.o

PAR_OBJS += pugixml.o
PAR_OBJS += AltoXmlParserLine.o
PAR_OBJS += AltoXmlPageParser.o
PAR_OBJS += ParserPage.o
PAR_OBJS += Xml.o
PAR_OBJS += XmlParserPage.o
PAR_OBJS += AbbyyXmlParserLine.o
PAR_OBJS += AbbyyXmlPageParser.o

TESTS += AltoXmlParsing.test
TESTS += AbbyyXmlParsing.test

default: all
all: pcwd

pcwd: pcwd.o libpcwapi.a libpcwcore.a libpcwparser.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

%.test: %.o libpcwapi.a libpcwcore.a libpcwparser.a libpcwcore.a
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -l boost_unit_test_framework

test: $(TESTS)
	@errors=0;\
	for test in $(TESTS); do \
		echo running $$test ;\
		./$$test; \
		res=$$?; \
		if [ $$res -ne 0 ]; then \
			errors=$$((errors + 1)); \
		fi \
	done; \
	exit $$errors

libpcwcore.a: $(OBJS)
	$(AR) rcs $@ $^
libpcwapi.a: $(API_OBJS)
	$(AR) rcs $@ $^
libpcwparser.a: $(PAR_OBJS)
	$(AR) rcs $@ $^

## libpcwcore.so: $(OBJS)
## 	$(CXX) $(CXXFLAGS) -shared -o $@ $^

.PHONY: clean
clean:
	$(RM) *.o *.d *.so *.a *.test pcwd

DEPS = $(patsubst %.o,%.d,$(OBJS) $(API_OBJS) $(PAR_OBJS) $(patsubst %.test,%.o,$(TESTS)) pcwd.o)
-include $(DEPS)

