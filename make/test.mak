UTILS_TESTS += rest/src/utils/tests/TestError.test
UTILS_TESTS += rest/src/utils/tests/TestMaybe.test
UTILS_TESTS += rest/src/utils/tests/TestScopeGuard.test

CORE_TESTS += rest/src/core/tests/TestBookBuilder.test
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
CORE_TESTS += rest/src/core/tests/TestPassword.test
CORE_TESTS += rest/src/core/tests/TestProjectBuilder.test
CORE_TESTS += rest/src/core/tests/TestSearcher.test
CORE_TESTS += rest/src/core/tests/TestSession.test
CORE_TESTS += rest/src/core/tests/TestSessionStore.test
CORE_TESTS += rest/src/core/tests/TestUtils.test
CORE_TESTS += rest/src/core/tests/TestWagnerFischer.test

DATABASE_TESTS += rest/src/database/tests/TestTables.test
DATABASE_TESTS += rest/src/database/tests/TestDatabase.test
DATABASE_TESTS += rest/src/database/tests/TestConnectionPool.test
DATABASE_TESTS += rest/src/database/tests/TestDatabaseGuard.test

PARSER_TESTS += rest/src/parser/tests/TestAltoXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestAbbyyXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestOcropusLlocsParsing.test
PARSER_TESTS += rest/src/parser/tests/TestHocrParsing.test
PARSER_TESTS += rest/src/parser/tests/TestBookDirectoryBuilder.test

PROFILER_TESTS += rest/src/profiler/tests/TestProfile.test
PROFILER_TESTS += rest/src/profiler/tests/TestDocXml.test

TESTS = $(UTILS_TESTS) $(CORE_TESTS) $(DATABASE_TESTS) $(PARSER_TESTS) $(PROFILER_TESTS)
RUN_TESTS = $(patsubst %.test,%.run,$(TESTS))
%.test: %.o
	$(call ECHO,$@)
	$V $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -l boost_unit_test_framework
%.run: %.test
	@$< > /dev/null 2>&1; \
	res=$$?; \
	if [ $$res -ne 0 ]; then \
		printf "%-60s \033[0;31mFAIL\033[0m\n" "$@:"; \
		exit 1; \
	else \
		printf "%-60s \033[0;32mSUCCESS\033[0m\n" "$@:"; \
	fi
test: $(RUN_TESTS)

DEPS += $(patsubst %.test,%.d,$(TESTS))
.PHONY: test
