TESTS += rest/src/tests/TestError.test
TESTS += rest/src/tests/TestMaybe.test
TESTS += rest/src/tests/TestConfig.test
TESTS += rest/src/tests/TestTables.test
TESTS += rest/src/tests/TestPassword.test
TESTS += rest/src/tests/TestOcrLine.test
TESTS += rest/src/tests/TestCorLine.test
TESTS += rest/src/tests/TestDocXml.test
TESTS += rest/src/tests/TestWagnerFischer.test
TESTS += rest/src/tests/TestCorrector.test
TESTS += rest/src/tests/TestAltoXmlParsing.test
TESTS += rest/src/tests/TestAbbyyXmlParsing.test
TESTS += rest/src/tests/TestOcropusLlocsParsing.test
TESTS += rest/src/tests/TestHocrParsing.test
TESTS += rest/src/tests/TestBookDirectoryBuilder.test
TESTS += rest/src/tests/TestProfile.test

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
