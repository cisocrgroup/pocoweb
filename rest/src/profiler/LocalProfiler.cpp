#include "LocalProfiler.hpp"
#include "Profile.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "docxml.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <crow/logging.h>
#include <cstdlib>
#include <fstream>
#include <thread>

using namespace pcw;
namespace fs = boost::filesystem;

////////////////////////////////////////////////////////////////////////////////
LocalProfiler::LocalProfiler(ConstBookSptr book, const Config &config)
    : Profiler(std::move(book)), tmpdir_(), langdir_(config.profiler.langdir),
      exe_(config.profiler.exe), outfile_(tmpdir_ / "out.xml"),
      infile_(tmpdir_ / "in.xml"), debug_(config.profiler.debug) {}

////////////////////////////////////////////////////////////////////////////////
Profile LocalProfiler::do_profile() {
  CROW_LOG_INFO << "(LocalProfiler) Profiling book id: " << book().id();
  write_docxml();
  run_profiler(profiler_command());
  CROW_LOG_INFO << "(LocalProfiler) Done profiling book id: " << book().id();
  return read_profile();
}

////////////////////////////////////////////////////////////////////////////////
void LocalProfiler::run_profiler(const std::string &command) const {
  CROW_LOG_DEBUG << "(LocalProfiler) Executing profiler command: " << command;
  auto res = system(command.data());
  CROW_LOG_DEBUG << "(LocalProfiler) Return code: " << res;
  switch (res) {
  case -1:
    throw std::system_error(errno, std::system_category(), "fork");
  case 127:
    THROW(Error, "Shell could not execute profiler: ", command);
  case 0:
    break;
  default:
    THROW(Error, "Profiler returned error code: ", res);
  }
}

////////////////////////////////////////////////////////////////////////////////
Profile LocalProfiler::read_profile() const {
  CROW_LOG_DEBUG << "(LocalProfiler) Reading profile file " << infile_;
  ProfileBuilder builder(book().book_ptr());
  builder.add_candidates_from_file(infile_);
  if (debug_) { // write tmp-profile if debugging is enabled
    boost::system::error_code ec;
    hard_link_or_copy(infile_, "profile-tmp.xml", ec);
    if (ec) {
      CROW_LOG_WARNING << "(LocalProfiler) could not create profile-tmp: "
                       << ec.message();
    }
  }
  CROW_LOG_DEBUG << "(LocalProfiler) Done reading profile file " << infile_;
  return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
void LocalProfiler::write_docxml() const {
  CROW_LOG_DEBUG << "(LocalProfiler) Writing doc xml file " << outfile_;
  DocXml docxml;
  docxml << book();
  std::ofstream os(outfile_.string());
  if (not os.good())
    throw std::system_error(errno, std::system_category(), outfile_.string());
  docxml.save(os);
  os.close();
  CROW_LOG_DEBUG << "(LocalProfiler) Done writing doc xml file " << outfile_;
}

////////////////////////////////////////////////////////////////////////////////
std::string LocalProfiler::profiler_command() const {
  return exe_.string() + " --adaptive" + " --config " + profiler_config() +
         " --sourceFormat DocXML" + " --sourceFile " + outfile_.string() +
         " --out_doc " + infile_.string() +
         " 2>&1"; // redirect stderr to stdout
}

////////////////////////////////////////////////////////////////////////////////
std::string LocalProfiler::profiler_config() const {
  return (langdir_ / (book().data.lang + ".ini")).string();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> LocalProfiler::do_languages() {
  std::vector<std::string> languages;
  fs::directory_iterator b(langdir_), e;
  BOOST_FOREACH (const fs::path &p, std::make_pair(b, e)) {
    if (fs::is_regular_file(p) and p.extension() == ".ini") {
      languages.push_back(p.filename().replace_extension().string());
    }
  }
  return languages;
}

////////////////////////////////////////////////////////////////////////////////
// std::string
// LocalProfiler::exec(const std::string& command)
// {
// 	std::shared_ptr<FILE> pipe(popen(command.data(), "r"), pclose);
// 	std::string res;
// 	char buffer[1024];
// 	if (not pipe)
// 		throw std::system_error(errno, std::system_category(), "popen");
// 	while (not feof(pipe.get())) {
// 		if (fgets(buffer, 1024, pipe.get()))
// 			res += buffer;
// 	}
// 	return res;
// }
