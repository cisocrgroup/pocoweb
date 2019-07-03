UTILS_TESTS += rest/src/utils/tests/TestBase64.test
UTILS_TESTS += rest/src/utils/tests/TestError.test
UTILS_TESTS += rest/src/utils/tests/TestMaybe.test
UTILS_TESTS += rest/src/utils/tests/TestScopeGuard.test
UTILS_TESTS += rest/src/utils/tests/TestUniqueIdMap.test

CORE_TESTS += rest/src/core/tests/TestBookBuilder.test
CORE_TESTS += rest/src/core/tests/TestBookDirectoryBuilder.test
CORE_TESTS += rest/src/core/tests/TestBox.test
CORE_TESTS += rest/src/core/tests/TestCache.test
CORE_TESTS += rest/src/core/tests/TestCorLine.test
CORE_TESTS += rest/src/core/tests/TestCorrector.test
CORE_TESTS += rest/src/core/tests/TestJson.test
CORE_TESTS += rest/src/core/tests/TestLine.test
CORE_TESTS += rest/src/core/tests/TestLineBuilder.test
CORE_TESTS += rest/src/core/tests/TestOcrLine.test
CORE_TESTS += rest/src/core/tests/TestPackageBuilder.test
CORE_TESTS += rest/src/core/tests/TestPageBuilder.test
CORE_TESTS += rest/src/core/tests/TestProjectBuilder.test
CORE_TESTS += rest/src/core/tests/TestQueries.test
CORE_TESTS += rest/src/core/tests/TestSearcher.test
CORE_TESTS += rest/src/core/tests/TestSession.test
CORE_TESTS += rest/src/core/tests/TestUtils.test
CORE_TESTS += rest/src/core/tests/TestWagnerFischer.test

DATABASE_TESTS += rest/src/database/tests/TestDatabase.test
DATABASE_TESTS += rest/src/database/tests/TestConnectionPool.test
DATABASE_TESTS += rest/src/database/tests/TestDatabaseGuard.test

PARSER_TESTS += rest/src/parser/tests/TestAbbyyXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestAltoXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestBookDirectoryBuilder.test
PARSER_TESTS += rest/src/parser/tests/TestHocrParsing.test
PARSER_TESTS += rest/src/parser/tests/TestOcropusLlocsParsing.test
PARSER_TESTS += rest/src/parser/tests/TestPageXmlParsing.test

PROFILER_TESTS += rest/src/profiler/tests/TestDocXml.test
PROFILER_TESTS += rest/src/profiler/tests/TestProfile.test
PROFILER_TESTS += rest/src/profiler/tests/TestRemoteProfiler.test
PROFILER_TESTS += rest/src/profiler/tests/TestRemoteProfilerTemplate.test

TESTS = $(UTILS_TESTS) $(CORE_TESTS) $(DATABASE_TESTS) $(PARSER_TESTS) $(PROFILER_TESTS)
RUN_TESTS = $(patsubst %.test,%.run,$(TESTS))
%.test: %.o $(LIBS)
	$V $(call ECHO,$@)
	$V $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -l boost_unit_test_framework
%.run: %.test
	$V $(call ECHO,$@)
#@echo "# $@" >> tests.log
	$V $< --color_output=false
#test: tests.log $(RUN_TESTS)
test: $(RUN_TESTS)
tests.log:
#	@echo "# tests:" $(shell date) > $@

DEPS += $(patsubst %.test,%.d,$(TESTS))
TESTOBJS += $(patsubst %.test,%.o,$(TESTS))
.PHONY: test tests.log
