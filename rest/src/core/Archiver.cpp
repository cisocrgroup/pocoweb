#include "Archiver.hpp"
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include "WagnerFischer.hpp"
#include "database/DbStructs.hpp"
#include "database/Tables.h"
#include "parser/PageParser.hpp"
#include "parser/ParserPage.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <boost/filesystem/operations.hpp>
#include <cerrno>
#include <crow/logging.h>
#include <fstream>
#include <regex>
#include <system_error>

using namespace pcw;
namespace fs = boost::filesystem;

////////////////////////////////////////////////////////////////////////////////
Archiver::Archiver(int pid, MysqlConnection &conn, const Config &config)
    : conn_(conn), basedir_(config.daemon.projectdir), pid_(pid) {}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::operator()() const {
  // const auto oldwd = fs::current_path();
  // ScopeGuard restoreoldwd([&oldwd]() { fs::current_path(oldwd); });
  // fs::current_path(basedir_);
  DbPackage package(pid_);
  if (not package.load(conn_)) {
    THROW(Error, "cannot load package: ", pid_);
  }
  const auto archive =
      basedir_.parent_path() / package.directory / archive_name(package);
  const auto dir = archive.parent_path() / archive.stem();
  ScopeGuard deltmpdir([&dir]() { fs::remove_all(dir); });
  ScopeGuard delarchive([&archive]() { fs::remove(archive); });
  copy_files(dir, package);
  zip(dir, archive);
  delarchive.dismiss();
  // do *not* dismiss deltmpdir; it should be removed all the time.
  // // do *not* dissmiss restoreoldwd, since we want change back to it in
  // any case.
  return package.directory / archive.filename();
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::zip(const Path &xdir, const Path &archive) const {
  const auto base = basedir_.parent_path();
  const auto dir = base / xdir;
  const auto old = fs::current_path();
  ScopeGuard restoreold([&old]() { fs::current_path(old); });
  auto new__ = dir.parent_path();
  // change into the directory to fix file paths in the resulting zip
  // archive.
  fs::current_path(new__);
  const auto command = "zip -r -qq " + archive.filename().string() + " " +
                       dir.filename().string();
  CROW_LOG_INFO << "(Archiver) zip command: " << command;
  const auto err = system(command.data());
  if (err)
    THROW(Error, "Cannot unzip file: `", command, "` returned: ", err);
  // do *not* dismiss restoreold, since we want to change back to the old
  // working directory.
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::copy_files(const Path &xdir, const DbPackage &package) const {
  const auto base = basedir_.parent_path();
  const auto dir = base / xdir;
  CROW_LOG_INFO << "(Archiver) dir:        " << dir;
  WagnerFischer wf;
  for (const auto pageid : package.pageids) {
    DbPage page(pid_, pageid);
    if (not page.load(conn_)) {
      THROW(Error, "cannot load page: ", pid_, ":", pageid);
    }
    CROW_LOG_INFO << "(Archiver) ocr path:  " << page.ocrpath;
    CROW_LOG_INFO << "(Archiver) base dir:  " << base;
    if (page.ocrpath.empty()) {
      CROW_LOG_WARNING << "(Archiver) page: " << pid_ << ":" << pageid
                       << " has no associated ocr path";
      continue;
    }
    const auto path = base / page.ocrpath;
    CROW_LOG_INFO << "(Archiver) file path: " << path;
    const auto pp =
        make_page_parser(FileType(page.filetype), base / page.ocrpath)->parse();
    for (auto &line : page.lines) {
      if (line.imagepath.empty()) {
        CROW_LOG_WARNING << "(Archiver) line: " << pid_ << ":" << pageid << ":"
                         << line.lineid << " has no image path";
        continue;
      }
      wf.set_gt(line.slice().wcor());
      // page parser starts with 0,
      // line ids start at 1.
      assert(line.lineid > 0);
      pp->get(line.lineid - 1).correct(wf);
      const auto img = copy_to_tmp_dir(base / line.imagepath, dir);
      const auto cor =
          img.parent_path() / img.stem().replace_extension(".gt.txt");
      const auto ocr = img.parent_path() / img.stem().replace_extension(".txt");
      CROW_LOG_INFO << "(Archiver) paths: " << cor << " " << ocr;
      write_line_text_snippets(line, cor, ocr);
    }
    pp->write(get_tmp_file(page.ocrpath, dir));
    if (not page.imagepath.empty()) {
      CROW_LOG_INFO << "(Archiver) img path:  " << base / page.imagepath;
      CROW_LOG_INFO << "(Archiver) copy to:   " << dir;
      copy_to_tmp_dir(base / page.imagepath, dir);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::get_tmp_file(const Path &source,
                                      const Path &tmpdir) const {
  const auto base = remove_common_base_path(source.parent_path(), tmpdir);
  const auto target = tmpdir / base / source.filename();
  return target;
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::copy_to_tmp_dir(const Path &source,
                                         const Path &tmpdir) const {
  const auto target = get_tmp_file(source, tmpdir);
  boost::system::error_code ec;
  fs::create_directories(target.parent_path(), ec);
  if (ec) {
    THROW(Error, "(Archiver) cannot create directory: ", target.parent_path(),
          ": ", ec.message());
  }
  CROW_LOG_DEBUG << "(Archiver) copying " << source << " to " << target;
  hard_link_or_copy(source, target, ec);
  if (ec) {
    THROW(Error, "(Archiver) cannot copy ", source, " to ", target, ": ",
          ec.message());
  }
  return target;
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::write_line_text_snippets(DbLine &line, const Path &cor,
                                        const Path &ocr) {
  const auto slice = line.slice();
  if (slice.is_fully_corrected()) {
    write_text_snippet(slice.cor(), cor);
  }
  write_text_snippet(slice.ocr(), ocr);
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::write_text_snippet(const std::string &line, const Path &to) {
  std::ofstream os(to.string());
  if (not os.good()) {
    throw std::system_error(errno, std::system_category(), to.string());
  }
  os << line << std::endl;
  os.close();
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::archive_name(const DbPackage &package) noexcept {
  static const auto delre = std::regex(R"([<>:"'/\\|?*]+)");
  static const auto replre = std::regex(R"(\s+)");
  const auto dir = Path(package.directory);
  auto basename = dir.filename().string() + "_" + package.author + "_" +
                  package.title + "_" + std::to_string(package.year) + ".zip";
  basename = std::regex_replace(basename, delre, "");
  basename = std::regex_replace(basename, replre, "_");
  return basename;
}

////////////////////////////////////////////////////////////////////////////////
Path Archiver::remove_common_base_path(const Path &p, const Path &base) {
  auto i = std::mismatch(p.begin(), p.end(), base.begin(), base.end());
  if (i.first != p.end()) {
    Path res;
    for (auto j = i.first; j != p.end(); ++j) {
      res /= *j;
    }
    return res;
  }
  return p;
}
