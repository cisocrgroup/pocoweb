#include <boost/filesystem/operations.hpp>
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
static void create_base_directory(const Config& config);

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
	create_base_directory(app.config());
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
	const auto pw = getpwnam(user);
	if (not pw and errno)
		throw std::system_error(errno, std::system_category(), user);
	else if (not pw)
		THROW(Error, "(pcwd) Could not find user: ", user);

	// set processes uid and gid
	if (setuid(pw->pw_uid) != 0)
		throw std::system_error(errno, std::system_category(), "setuid");
	if (setgid(pw->pw_gid) != 0)
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

////////////////////////////////////////////////////////////////////////////////
void
create_base_directory(const Config& config)
{
	boost::system::error_code ec;
	if (not boost::filesystem::create_directories(config.daemon.basedir, ec)
			and ec.value() != 0) {
		throw std::system_error(ec.value(), std::system_category(),
				config.daemon.basedir);
	}
}
