#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ConfigTest

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "core/Config.hpp"

using namespace pcw;

struct ConfigFixture {
	static const char* INI;
	ConfigFixture(): is(INI), config(Config::load(is)) {
		BOOST_REQUIRE_EQUAL(config.plugins.configs.size(), 2);
	}

	std::stringstream is;
	Config config;
};
const char* ConfigFixture::INI =
R"(
[db]
host = db.host
user = ${daemon.user}
pass = db.pass
db = db.db

[daemon]
host = daemon.host
port = 8080
threads = 4
detach = true
user = daemon.user
group = ${db.user}
sessions = 100
basedir = ${HOME}

[log]
file = log.file
# available levels: debug,info,warning,error,critical; default is info
level = debug

[plugin-first]
val1 = plugin-first.val1
val2 = plugin-first.val2
home = ${daemon.basedir}

[plugin-second]
val1 = plugin-second.val1
val2 = plugin-second.val2
home = ${plugin-first.home}
)";

////////////////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ConfigTest, ConfigFixture)

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestConfigDb)
{
	BOOST_CHECK_EQUAL(config.db.host, "db.host");
	BOOST_CHECK_EQUAL(config.db.user, "daemon.user");
	BOOST_CHECK_EQUAL(config.db.pass, "db.pass");
	BOOST_CHECK_EQUAL(config.db.db, "db.db");
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestConfigDaemon)
{
	BOOST_CHECK_EQUAL(config.daemon.host, "daemon.host");
	BOOST_CHECK_EQUAL(config.daemon.port, 8080);
	BOOST_CHECK_EQUAL(config.daemon.threads, 4);
	BOOST_CHECK_EQUAL(config.daemon.detach, true);
	BOOST_CHECK_EQUAL(config.daemon.user, "daemon.user");
	BOOST_CHECK_EQUAL(config.daemon.group, "daemon.user");
	BOOST_CHECK_EQUAL(config.daemon.sessions, 100);
	BOOST_CHECK_EQUAL(config.daemon.basedir, getenv("HOME"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestConfigFirstPlugin)
{
	BOOST_CHECK_EQUAL(config.plugins["first"].get<std::string>("val1"),
			"plugin-first.val1");
	BOOST_CHECK_EQUAL(config.plugins["first"].get<std::string>("val2"),
			"plugin-first.val2");
	BOOST_CHECK_EQUAL(config.plugins["first"].get<std::string>("home"),
			getenv("HOME"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(TestConfigSecondPlugin)
{
	BOOST_CHECK_EQUAL(config.plugins["second"].get<std::string>("val1"),
			"plugin-second.val1");
	BOOST_CHECK_EQUAL(config.plugins["second"].get<std::string>("val2"),
			"plugin-second.val2");
	BOOST_CHECK_EQUAL(config.plugins["second"].get<std::string>("home"),
			getenv("HOME"));
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()
