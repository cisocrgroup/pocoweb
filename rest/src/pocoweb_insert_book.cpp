#include <cassert>
#include <memory>
#include <vector>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "doc/hocr.hpp"
#include "doc/Document.hpp"
#include "util/Config.hpp"

using namespace pcw;
static Config loadConfig(int argc, char **argv);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	try {
		auto config = loadConfig(argc, argv);
		auto book = parse_hocr(argv[8]);
		book->author = argv[4];
		book->title = argv[5];
		book->year = std::stoi(argv[6]);
		book->desc = argv[7];
		book->uri = argv[8];
		
		for (const auto& page: *book) {
			for (const auto& line: *page) {
				std::cout << page->id << " " << line->id << " " << line->line();
				for (int c: line->cuts()) 
					std::cout << " " << c;
				std::cout << "\n";
			}
		}
		//auto conn = connect(config);
		//DbTableUsers db(conn);
		//auto user = db.createUser(argv[2], argv[3], argv[4], argv[5]);
		return EXIT_SUCCESS; 
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	}
	return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
Config
loadConfig(int argc, char **argv)
{
	if (argc < 9)
		throw std::runtime_error(std::string("Usage: ") +
					 argv[0] +
					 " config-file type owner author title year desc path [users...]");

	Config config;
	config.load(argv[1]);
	return config;
}
