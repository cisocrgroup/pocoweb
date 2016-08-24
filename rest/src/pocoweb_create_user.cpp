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
#include "Config.hpp"

using namespace pcw;
static Config loadConfig(int argc, char **argv);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	try {
		auto config = loadConfig(argc, argv);
		auto conn = connect(config);
		DbTableUsers db(std::move(conn));
		auto user = db.createUser(argv[2], argv[3], argv[4], argv[5]);
		return user ? EXIT_SUCCESS : EXIT_FAILURE;
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	}
	return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
Config
loadConfig(int argc, char **argv)
{
	if (argc < 6)
		throw std::runtime_error(std::string("Usage: ") +
					 argv[0] +
					 " config-file name email institute passwd");
	return Config::load(argv[1]);
}
