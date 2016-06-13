#include <cassert>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include "Config.hpp"
#include "db.hpp"
#include "User.hpp"
#include "DbTableUsers.hpp"

using namespace pcw;
static const char *def = "config.ini";

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	const char *configfile = def;
	if (argc > 1)
		configfile = argv[1];
	Config config;
	config.load(configfile);

	ConnectionPtr conn{get_driver_instance()->connect(config.dbHost,
							  config.dbUser,
							  config.dbPass)};
	DbTableUsers db(conn);
	auto user = db.findUserByEmail("finkf@cis.lmu.de");
	if (user)
		std::cout << *user << "\n";
}
