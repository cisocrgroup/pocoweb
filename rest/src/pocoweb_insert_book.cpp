#include <cassert>
#include <memory>
#include <vector>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <json.hpp>
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"
#include "db/DbTableBooks.hpp"
#include "doc/hocr.hpp"
#include "doc/Box.hpp"
#include "doc/BookData.hpp"
#include "doc/Container.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "doc/Book.hpp"
#include "util/Config.hpp"

using namespace pcw;
static Config loadConfig(int argc, char **argv);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	try {
		auto config = loadConfig(argc, argv);
		auto book = parse_hocr(argv[9]);
		book->data.author = argv[4];
		book->data.title = argv[5];
		book->data.year = std::stoi(argv[6]);
		book->data.desc = argv[7];
		book->data.uri = argv[8];
		book->data.path = argv[9];
		book->data.firstpage = book->empty() ? 0 : 1;
		book->data.lastpage = static_cast<int>(book->size());
		
		const auto conn = connect(config);
		DbTableUsers users(conn);
		const auto owner = users.findUserByNameOrEmail(argv[3]);
		if (not owner)
			throw std::runtime_error(
				"invalid book owner: " + 
				std::string(argv[3])
			);
		DbTableBooks books(conn);
		auto res = books.insertBook(owner->id, *book);
		if (res) {
			for (int i = 10; i < argc; ++i) {
				auto user = users.findUserByNameOrEmail(argv[i]);
				if (not user)
					throw std::runtime_error(
						"No such user: " + 
						std::string(argv[i])
					);
				books.allowBook(user->id, book->id);
			}
		}
		return res ? EXIT_SUCCESS : EXIT_FAILURE;
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	}
	return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
Config
loadConfig(int argc, char **argv)
{
	if (argc < 10)
		throw std::runtime_error(std::string("Usage: ") +
					 argv[0] +
					 " config-file type owner author title "
					 "year desc uri path [users...]");

	Config config;
	config.load(argv[1]);
	return config;
}
