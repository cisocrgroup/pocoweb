#include <cassert>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <boost/log/trivial.hpp>
#include "server_http.hpp"
#include "util/Config.hpp"
#include "api/api.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

using namespace pcw;
static const char *def = "config.ini";
static Config loadConfig(int argc, char **argv);
static void init(const Config& config);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	const auto config = loadConfig(argc, argv);
	init(config);

	Api::Server server(config.daemonPort, config.daemonThreads);
	server.resource["^/login$"]["GET"] = Login(config);
	std::thread sthread([&server](){
			BOOST_LOG_TRIVIAL(info) << "server starting";
			server.start();
			BOOST_LOG_TRIVIAL(info) << "server stopping";
		});
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

////////////////////////////////////////////////////////////////////////////////
Config
loadConfig(int argc, char **argv)
{
	const char *configfile = def;
	if (argc > 1)
		configfile = argv[1];
	Config config;
	config.load(configfile);
	return config;
}

////////////////////////////////////////////////////////////////////////////////
void
init(const Config& config)
{
	boost::log::add_file_log(config.logFile);
	boost::log::core::get()->set_filter(
	        boost::log::trivial::severity >= boost::log::trivial::info
	);
}
