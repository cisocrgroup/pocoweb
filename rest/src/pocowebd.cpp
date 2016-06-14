#include <cassert>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include "server_http.hpp"
#include "util/Config.hpp"
#include "api/api.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"

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

	Api::Server server(config.daemonPort, config.daemonThreads);
	server.resource["^/login$"]["POST"] = Login(config);

	std::thread sthread([&server](){server.start();});
	sthread.join();
	// std::string

	// ConnectionPtr conn{get_driver_instance()->connect(config.dbHost,
	// 						  config.dbUser,
	// 						  config.dbPass)};
	// DbTableUsers db(conn);
	// auto user = db.findUserByEmail("finkf@cis.lmu.de");
	// if (user)
	// 	std::cout << *user << "\n";
	// auto users = db.findUsers([](const auto&){return true;});
	// for (const auto& user: users)
	// 	std::cout << "user: " << *user << "\n";
}
