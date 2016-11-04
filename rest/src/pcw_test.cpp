#include <functional>
#include <iostream>
#include <vector>
#include "parser/AltoXmlPageParser.hpp"
#include "parser/ParserChar.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static int 
test_alto_xml_parser()
{
	AltoXmlPageParser p("misc/data/test/alto-test.xml");
	while (p.has_next()) {
		auto page = p.pparse();
		for (const auto& line: page.lines) {
			for (const auto& c: line.chars) {
				assert(c);
				std::wcerr << c->get();
			}
		}
		std::wcerr << "\n";
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
static const std::vector<std::function<int()>> TESTS {
	test_alto_xml_parser,
};

////////////////////////////////////////////////////////////////////////////////
int
main()
{
	for (const auto& test: TESTS) {
		test();	
	}
}
