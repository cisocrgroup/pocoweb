#include "api/AssignRoute.hpp"
#include "api/BookRoute.hpp"
#include "api/DownloadRoute.hpp"
#include "api/FinishRoute.hpp"
#include "api/LineRoute.hpp"
#include "api/PageRoute.hpp"
#include "api/SearchRoute.hpp"
#include "api/SplitImagesRoute.hpp"
#include "api/SplitRoute.hpp"
#include "api/TokenRoute.hpp"
#include "api/VersionRoute.hpp"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "core/Route.hpp"
#include "crow.h"
#include <boost/filesystem/operations.hpp>
#include <grp.h>
#include <iostream>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace pcw;
using AppPtr = std::unique_ptr<App>;
static int run(int argc, char **argv);
static void run(App &app);
static AppPtr get_app(int argc, char **argv);
static void change_user_and_group(const Config &config);
static void detach(const Config &config);
static void create_base_directory(const Config &config);
static const char *find_config_file(int argc, char **argv);
static void write_pidfile(const Config &config);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  try {
    return run(argc, argv);
  } catch (const std::exception &e) {
    CROW_LOG_ERROR << "fatal: " << e.what();
  }
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
int run(int argc, char **argv) {
  auto app = get_app(argc, argv);
  run(*app);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void run(App &app) {
  change_user_and_group(app.config());
  create_base_directory(app.config());
  detach(app.config());
  app.Register(std::make_unique<pcw::AssignRoute>());
  app.Register(std::make_unique<pcw::BookRoute>());
  app.Register(std::make_unique<pcw::DownloadRoute>());
  app.Register(std::make_unique<pcw::FinishRoute>());
  app.Register(std::make_unique<pcw::LineRoute>());
  app.Register(std::make_unique<pcw::PageRoute>());
  app.Register(std::make_unique<pcw::SearchRoute>());
  app.Register(std::make_unique<pcw::SplitImagesRoute>());
  app.Register(std::make_unique<pcw::SplitRoute>());
  app.Register(std::make_unique<pcw::TokenRoute>());
  app.Register(std::make_unique<pcw::VersionRoute>());
  app.run();
}

////////////////////////////////////////////////////////////////////////////////
void change_user_and_group(const Config &config) {
  const auto user = config.daemon.user.data();
  errno = 0;
  const auto pw = getpwnam(user);
  if (not pw and errno)
    throw std::system_error(errno, std::system_category(), user);
  else if (not pw)
    THROW(Error, "(pcwd) Could not find user: ", user);

  // set processes uid and gid
  if (setgid(pw->pw_gid) != 0)
    throw std::system_error(errno, std::system_category(), "setgid");
  if (setuid(pw->pw_uid) != 0)
    throw std::system_error(errno, std::system_category(), "setuid");
}

////////////////////////////////////////////////////////////////////////////////
void detach(const Config &config) {
  if (config.daemon.detach) {
    write_pidfile(config);
    if (daemon(0, 0) != 0) {
      throw std::system_error(errno, std::system_category(), "daemon");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
AppPtr get_app(int argc, char **argv) {
  auto app = std::make_unique<pcw::App>(find_config_file(argc, argv));
  app->config().setup_logging();
  app->config().LOG();
  return app;
}

////////////////////////////////////////////////////////////////////////////////
void create_base_directory(const Config &config) {
  boost::system::error_code ec;
  const auto dir = config.daemon.basedir + "/" + config.daemon.projectdir;
  if (not boost::filesystem::create_directories(dir, ec) and ec.value() != 0) {
    throw std::system_error(ec.value(), std::system_category(),
                            config.daemon.basedir);
  }
}

////////////////////////////////////////////////////////////////////////////////
const char *find_config_file(int argc, char **argv) {
  if (not(argc > 1)) {
    throw std::runtime_error("usage: " + std::string(argv[0]) + " config");
  }
  return argv[1];
}

////////////////////////////////////////////////////////////////////////////////
void write_pidfile(const Config &config) {
  std::ofstream out(config.daemon.pidfile);
  if (not out.good()) {
    throw std::system_error(errno, std::system_category(),
                            config.daemon.pidfile);
  }
  out << getpid() << std::endl;
  out.close();
}
