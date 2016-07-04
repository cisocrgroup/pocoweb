#include <vector>
#include <fstream>
#include <json.hpp>
#include "doc/Box.hpp"
#include "doc/Container.hpp"
#include "doc/BookData.hpp"
#include "doc/Book.hpp"
#include "doc/Page.hpp"
#include "doc/Line.hpp"
#include "ex/Ex.hpp"

using namespace nlohmann;

int
main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "[error] Usage: " << argv[0] << " file\n";
		return 1;
	}
	std::ifstream is(argv[1]);
	if (not is.good()) {
		std::cerr << "[error] could not read file `" << argv[1] << "`\n";
		return 1;
	}
		
	json j;
	is >> j;
	std::vector<pcw::LinePtr> lines;
	for (auto& line: j["lines"]) 
		lines.push_back(std::make_shared<pcw::Line>(line));
	for (const auto& line: lines)
		std::cout << "line: " << line->line() << "\n";
	std::string line;	
	std::cin.clear();
	while (std::getline(std::cin, line)) {
		std::cout << "line: " << line << "\n";
		pcw::Ex ex(line);
		auto n = std::min(ex.address.second, lines.size());
		for (auto i = ex.address.first; i < n; ++i) {
			std::cout << lines[i]->line() << "\n";
		}

	}
}
