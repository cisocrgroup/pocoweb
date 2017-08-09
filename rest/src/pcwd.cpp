#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <vector>
#include "api/AssignRoute.hpp"
#include "api/BookRoute.hpp"
#include "api/CorrectionRoute.hpp"
#include "api/DownloadRoute.hpp"
#include "api/ErrorPatternsRoute.hpp"
#include "api/FinishRoute.hpp"
#include "api/LineRoute.hpp"
#include "api/PageRoute.hpp"
#include "api/ProfilerRoute.hpp"
#include "api/SearchRoute.hpp"
#include "api/SplitImagesRoute.hpp"
#include "api/SplitRoute.hpp"
#include "api/SuggestionsRoute.hpp"
#include "api/UserRoute.hpp"
#include "api/VersionRoute.hpp"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "core/Plugin.hpp"
#include "core/Route.hpp"
#include "crow.h"

using namespace pcw;
using AppPtr = std::unique_ptr<App>;
static int run(int argc, char** argv);
static void run(App& app);
static AppPtr get_app(int argc, char** argv);
static void change_user_and_group(const Config& config);
static void create_base_directory(const Config& config);
static const char* find_config_file(int argc, char** argv);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
	try {
		return run(argc, argv);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << e.what();
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
int run(int argc, char** argv) {
	auto app = get_app(argc, argv);
	run(*app);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void run(App& app) {
	change_user_and_group(app.config());
	create_base_directory(app.config());
	app.register_plugins();
	app.Register(std::make_unique<pcw::AssignRoute>());
	app.Register(std::make_unique<pcw::BookRoute>());
	app.Register(std::make_unique<pcw::CorrectionRoute>());
	app.Register(std::make_unique<pcw::DownloadRoute>());
	app.Register(std::make_unique<pcw::ErrorPatternsRoute>());
	app.Register(std::make_unique<pcw::FinishRoute>());
	app.Register(std::make_unique<pcw::LineRoute>());
	app.Register(std::make_unique<pcw::PageRoute>());
	app.Register(std::make_unique<pcw::ProfilerRoute>());
	app.Register(std::make_unique<pcw::SearchRoute>());
	app.Register(std::make_unique<pcw::SplitImagesRoute>());
	app.Register(std::make_unique<pcw::SplitRoute>());
	app.Register(std::make_unique<pcw::SuggestionsRoute>());
	app.Register(std::make_unique<pcw::UserRoute>());
	app.Register(std::make_unique<pcw::VersionRoute>());
	app.run();
}

////////////////////////////////////////////////////////////////////////////////
void change_user_and_group(const Config& config) {
	const auto user = config.daemon.user.data();
	errno = 0;
	const auto pw = getpwnam(user);
	if (not pw and errno)
		throw std::system_error(errno, std::system_category(), user);
	else if (not pw)
		THROW(Error, "(pcwd) Could not find user: ", user);

	// set processes uid and gid
	if (setuid(pw->pw_uid) != 0)
		throw std::system_error(errno, std::system_category(),
					"setuid");
	if (setgid(pw->pw_gid) != 0)
		throw std::system_error(errno, std::system_category(),
					"setgid");
}

////////////////////////////////////////////////////////////////////////////////
AppPtr get_app(int argc, char** argv) {
	auto app = std::make_unique<pcw::App>(find_config_file(argc, argv));
	app->config().setup_logging();
	app->config().LOG();
	return app;
}

////////////////////////////////////////////////////////////////////////////////
void create_base_directory(const Config& config) {
	boost::system::error_code ec;
	if (not boost::filesystem::create_directories(config.daemon.basedir,
						      ec) and
	    ec.value() != 0) {
		throw std::system_error(ec.value(), std::system_category(),
					config.daemon.basedir);
	}
}

////////////////////////////////////////////////////////////////////////////////
const char* find_config_file(int argc, char** argv) {
	// if a config file is given use it.
	if (argc > 1) {
		std::cerr << "(pcwd) using config file: " << argv[1] << "\n";
		return argv[1];
	}
	// search for possible default config files.
	static const char* files[] = {
#ifdef PCW_DESTDIR
	    PCW_DESTDIR "/etc/pcwd.ini",
#else   // PCWD_DESTDIR
	    "/etc/pcwd.ini",
#endif  // PCW_DESTDIR
	    "./pcwd.ini",
	};
	struct stat s;
	for (const auto file : files) {
		std::cerr << "(pcwd) trying config file: " << file << "\n";
		if (stat(file, &s) == 0) {
			std::cerr << "(pcwd) using config file: " << file
				  << "\n";
			return file;
		}
	}
	throw std::runtime_error("Cannot find config file");
}
