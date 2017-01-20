CORE_TESTS += rest/src/core/tests/TestError.test
CORE_TESTS += rest/src/core/tests/TestMaybe.test
CORE_TESTS += rest/src/core/tests/TestScopeGuard.test
CORE_TESTS += rest/src/core/tests/TestConfig.test
CORE_TESTS += rest/src/core/tests/TestPassword.test
CORE_TESTS += rest/src/core/tests/TestWagnerFischer.test
CORE_TESTS += rest/src/core/tests/TestBox.test
CORE_TESTS += rest/src/core/tests/TestOcrLine.test
CORE_TESTS += rest/src/core/tests/TestCorLine.test
CORE_TESTS += rest/src/core/tests/TestCorrector.test
CORE_TESTS += rest/src/core/tests/TestProfile.test
CORE_TESTS += rest/src/core/tests/TestLineBuilder.test
CORE_TESTS += rest/src/core/tests/TestPageBuilder.test
CORE_TESTS += rest/src/core/tests/TestBookBuilder.test
CORE_TESTS += rest/src/core/tests/TestProjectBuilder.test

DATABASE_TESTS += rest/src/database/tests/TestTables.test
DATABASE_TESTS += rest/src/database/tests/TestDatabase.test
DATABASE_TESTS += rest/src/database/tests/TestConnectionPool.test

PARSER_TESTS += rest/src/parser/tests/TestDocXml.test
PARSER_TESTS += rest/src/parser/tests/TestAltoXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestAbbyyXmlParsing.test
PARSER_TESTS += rest/src/parser/tests/TestOcropusLlocsParsing.test
PARSER_TESTS += rest/src/parser/tests/TestHocrParsing.test
PARSER_TESTS += rest/src/parser/tests/TestBookDirectoryBuilder.test

TESTS = $(CORE_TESTS) $(DATABASE_TESTS) $(PARSER_TESTS)
%.test: %.cpp $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -l boost_unit_test_framework

test: $(TESTS)
	@errors=0;\
	for test in $(TESTS); do \
		$$test > /dev/null 2>&1; \
		res=$$?; \
		if [ $$res -ne 0 ]; then \
			errors=$$((errors + 1)); \
			echo $$test': \033[0;31mFAIL\033[0m' ;\
		else \
			echo $$test': \033[0;32mSUCCESS\033[0m' ;\
		fi \
	done ;\
	if [ $$errors -ne 0 ]; then \
		echo "\033[0;31m$$errors FAILURE(S)\033[0m" ;\
		exit $$errors ;\
	fi

DEPS += $(patsubst %.test,%.d,$(TESTS))
.PHONY: test
