#include <grp.h>
#include <pwd.h>
#include <iostream>
#include <vector>
#include "crow.h"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "core/Plugin.hpp"
#include "core/Route.hpp"
#include "api/VersionRoute.hpp"
#include "api/BookRoute.hpp"
#include "api/PageRoute.hpp"
#include "api/LineRoute.hpp"

using namespace pcw;
using AppPtr = std::unique_ptr<App>;
static int run(int argc, char** argv);
static void run(App& app);
static AppPtr get_app(int argc, char** argv);
static void change_user_and_group(const Config& config);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	try {
		return run(argc, argv);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << e.what();
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
int
run(int argc, char** argv)
{
	auto app = get_app(argc, argv);
	run(*app);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void
run(App& app)
{
	change_user_and_group(app.config());
	app.register_plugins();
	app.Register(std::make_unique<pcw::VersionRoute>());
	app.Register(std::make_unique<pcw::BookRoute>());
	app.Register(std::make_unique<pcw::PageRoute>());
	app.Register(std::make_unique<pcw::LineRoute>());
	app.run();
}

////////////////////////////////////////////////////////////////////////////////
void
change_user_and_group(const Config& config)
{
	const auto user = config.daemon.user.data();
	errno = 0;
	const auto upw = getpwnam(user);
	if (not upw and errno)
		throw std::system_error(errno, std::system_category(), user);
	else if (not upw)
		THROW(Error, "(pcwd) Could not find user: ", user);
	const auto group = config.daemon.group.data();
	errno = 0;
	const auto gpw = getgrnam(group);
	if (not gpw and errno)
		throw std::system_error(errno, std::system_category(), group);
	else if (not gpw)
		THROW(Error, "(pcwd) Could not find group: ", group);
	if (setuid(upw->pw_uid) != 0)
		throw std::system_error(errno, std::system_category(), "setuid");
	if (setgid(gpw->gr_gid) != 0)
		throw std::system_error(errno, std::system_category(), "setgid");
}

////////////////////////////////////////////////////////////////////////////////
AppPtr
get_app(int argc, char** argv)
{
	if (argc != 2)
		throw std::runtime_error("Usage: " + std::string(argv[0]) + " <config>");
	auto app = std::make_unique<pcw::App>(argv[1]);
	app->config().setup_logging();
	app->config().LOG();
	return app;
}

// #include <grp.h>
// #include <pwd.h>
// #include <cassert>
// #include <memory>
// #include <cppconn/driver.h>
// #include <cppconn/statement.h>
// #include <boost/log/trivial.hpp>
// #include <boost/shared_ptr.hpp>
// #include <boost/date_time/posix_time/posix_time_types.hpp>
// #include <boost/log/trivial.hpp>
// #include <boost/log/core.hpp>
// #include <boost/log/expressions.hpp>
// #include <boost/log/sources/logger.hpp>
// #include <boost/log/utility/setup/file.hpp>
// #include <boost/log/utility/setup/console.hpp>
// #include <boost/log/utility/setup/common_attributes.hpp>
// #include <boost/log/support/date_time.hpp>
// #include <boost/log/sinks/sync_frontend.hpp>
// #include <boost/log/sinks/text_file_backend.hpp>
// #include <boost/log/sinks/text_ostream_backend.hpp>
// #include <boost/log/attributes/named_scope.hpp>
// #include <boost/date_time/posix_time/posix_time_types.hpp>
// #include <boost/log/core.hpp>
// #include <boost/log/trivial.hpp>
// #include <boost/log/expressions.hpp>
// #include <boost/log/sinks/text_file_backend.hpp>
// #include <boost/log/utility/setup/file.hpp>
// #include <boost/log/utility/setup/common_attributes.hpp>
// #include <boost/log/sources/severity_logger.hpp>
// #include <boost/log/sources/record_ostream.hpp>
// #include <json.hpp>
// #include "server_http.hpp"
// #include "Config.hpp"
// #include "util/hash.hpp"
// #include "api/api.hpp"
// #include "db/db.hpp"
// #include "db/User.hpp"
// #include "db/DbTableUsers.hpp"
//
// using namespace pcw;
// static void run(const std::string& configfile);
// static void change_user_and_group(const Config& config);
// static void init_logging(const Config& config);
//
// ////////////////////////////////////////////////////////////////////////////////
// int
// main(int argc, char** argv)
// {
// 	try {
// 		if (argc != 2)
// 			throw std::runtime_error("Usage: " + std::string(argv[0]) + " config-file");
// 		run(argv[1]);
// 		return EXIT_SUCCESS;
// 	} catch (const std::exception& e) {
// 		std::cerr << "[error] " << e.what() << "\n";
// 	}
// 	return EXIT_FAILURE;
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void
// run(const std::string& configfile)
// {
// 	const auto config = std::make_shared<Config>(Config::load(configfile));
// 	init_logging(*config);
// 	if (config->daemon.detach) {
// 		change_user_and_group(*config);
// 		if (daemon(0, 0) == -1)
// 			throw std::system_error(errno, std::system_category(), "daemon");
// 	}
// 	run(config);
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void
// change_user_and_group(const Config& config)
// {
// 	const auto user = config.daemon.user.data();
// 	const auto upw = getpwnam(user);
// 	if (not upw)
// 		throw std::system_error(errno, std::system_category(), user);
// 	const auto group = config.daemon.group.data();
// 	const auto gpw = getgrnam(group);
// 	if (not gpw)
// 		throw std::system_error(errno, std::system_category(), group);
// 	// change user and group (not implemented)
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// void
// init_logging(const Config& config)
// {
// 	boost::log::add_common_attributes();
// 	auto fmtTimeStamp = boost::log::expressions::
// 		format_date_time<boost::posix_time::ptime
// 				 >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
// 	auto fmtThreadId = boost::log::expressions::
// 		attr<boost::log::attributes::current_thread_id::value_type
// 		     >("ThreadID");
// 	auto fmtSeverity = boost::log::expressions::
// 		attr<boost::log::trivial::severity_level
// 		     >("Severity");
// 	boost::log::formatter logFmt =
// 		boost::log::expressions::format("[%1%] [%2%] [%3%] %4%")
// 		% fmtTimeStamp
// 		% fmtThreadId
// 		% fmtSeverity
// 		% boost::log::expressions::smessage;
// 	auto fsink = boost::log::add_file_log(
// 	         boost::log::keywords::file_name = config.log.file,
// 		 boost::log::keywords::rotation_size = 10 * 1024 * 1024,
// 		 boost::log::keywords::auto_flush = true,
// 		 boost::log::keywords::open_mode = std::ios_base::app);
// 	fsink->set_formatter(logFmt);
// 	boost::log::core::get()->set_filter(
// 	         boost::log::trivial::severity >= config.log.level
// 	);
// }
