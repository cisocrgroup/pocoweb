#include "api/BookRoute.hpp"
#include "api/CharMapRoute.hpp"
#include "api/DownloadRoute.hpp"
#include "api/LineRoute.hpp"
#include "api/PageRoute.hpp"
#include "api/SearchRoute.hpp"
#include "api/StatisticsRoute.hpp"
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
static void create_base_directory(const Config &config);
static std::shared_ptr<Config> config(int argc, char **argv);

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
  create_base_directory(app.config());
  app.Register(std::make_unique<pcw::BookRoute>());
  app.Register(std::make_unique<pcw::CharMapRoute>());
  app.Register(std::make_unique<pcw::DownloadRoute>());
  app.Register(std::make_unique<pcw::LineRoute>());
  app.Register(std::make_unique<pcw::PageRoute>());
  app.Register(std::make_unique<pcw::SearchRoute>());
  app.Register(std::make_unique<pcw::StatisticsRoute>());
  app.Register(std::make_unique<pcw::VersionRoute>());
  app.run();
}

////////////////////////////////////////////////////////////////////////////////
AppPtr get_app(int argc, char **argv) {
  auto cnf = config(argc, argv);
  auto app = std::make_unique<pcw::App>(cnf);
  app->config().setup_logging();
  app->config().LOG();
  return app;
}

////////////////////////////////////////////////////////////////////////////////
void create_base_directory(const Config &config) {
  boost::system::error_code ec;
  const auto dir = config.daemon.projectdir;
  if (not boost::filesystem::create_directories(dir, ec) and ec.value() != 0) {
    throw std::system_error(ec.value(), std::system_category(),
                            config.daemon.projectdir);
  }
}

////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Config> config(int argc, char **argv) {
  const char *usage =
      "[-d] [-m mysql-host] [-p mysql-pass] [-u mysql-user] "
      "[-n mysql-table-name] [-l listen-host] [-b projects-dir]";
  const char *flags = "dm:p:n:u:l:b:";
  const char *mpass = "";
  const char *mhost = "pocoweb";
  const char *mname = "pocoweb";
  const char *muser = "pocoweb";
  const char *host = "0.0.0.0:80";
  const char *pdir = "/srv/pocoweb/project-data";
  bool debug = false;

  int c;
  while ((c = getopt(argc, argv, flags)) != -1) {
    switch (c) {
    case 'd':
      debug = true;
      break;
    case 'm':
      mhost = optarg;
      break;
    case 'p':
      mpass = optarg;
      break;
    case 'n':
      mname = optarg;
      break;
    case 'u':
      muser = optarg;
      break;
    case 'l':
      host = optarg;
      break;
    case 'b':
      pdir = optarg;
      break;
    default:
      throw std::runtime_error("Usage: " + std::string(argv[0]) + " " + usage);
    }
  }
  auto ret = std::make_shared<Config>(Config{
      {muser, mhost, mpass, mname, 10, false},
      {host, pdir},
      {debug ? static_cast<int>(crow::LogLevel::Debug)
             : static_cast<int>(crow::LogLevel::Info)},
  });
  return ret;
}
