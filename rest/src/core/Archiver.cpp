#include "Archiver.hpp"
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include "WagnerFischer.hpp"
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
Archiver::Archiver(const Project& project,
                   MysqlConnection& conn,
                   const Config& config)
  : project_(project.shared_from_this())
  , conn_(conn)
  , basedir_(config.daemon.basedir)
{}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path
Archiver::operator()() const
{
  assert(project_);
  const auto oldwd = fs::current_path();
  ScopeGuard restoreoldwd([&oldwd]() { fs::current_path(oldwd); });
  fs::current_path(basedir_);
  const auto archive = project_->origin().data.dir / archive_name();
  const auto dir = archive.parent_path() / archive.stem();
  ScopeGuard deltmpdir([&dir]() { fs::remove_all(dir); });
  ScopeGuard delarchive([&archive]() { fs::remove(archive); });
  copy_files(dir);
  write_adaptive_token_set(dir);
  zip(dir, archive);
  delarchive.dismiss();
  // do *not* dismiss deltmpdir; it should be removed all the time.
  // do *not* dissmiss restoreoldwd, since we want change back to it in
  // any case.
  return archive;
}

////////////////////////////////////////////////////////////////////////////////
void
Archiver::zip(const Path& dir, const Path& archive) const
{
  const auto old = fs::current_path();
  ScopeGuard restoreold([&old]() { fs::current_path(old); });
  auto new__ = dir.parent_path();
  // change into the directory to fix file paths in the resulting zip
  // archive.
  fs::current_path(new__);
  const auto command =
    "zip -r -qq " + archive.filename().string() + " " + dir.filename().string();
  CROW_LOG_DEBUG << "(Archiver) zip command: " << command;
  const auto err = system(command.data());
  if (err)
    THROW(Error, "Cannot unzip file: `", command, "` returned: ", err);
  // do *not* dismiss restoreold, since we want to change back to the old
  // working directory.
}

////////////////////////////////////////////////////////////////////////////////
void
Archiver::write_gt_file(const Line& line, const Path& to) const
{
  std::ofstream os(to.string());
  if (not os.good())
    throw std::system_error(errno, std::system_category(), to.string());
  os << line.cor() << std::endl;
  os.close();
}

////////////////////////////////////////////////////////////////////////////////
void
Archiver::copy_files(const Path& dir) const
{
  assert(project_);
  WagnerFischer wf;
  for (const auto& page : *project_) {
    if (not page->has_ocr_path()) {
      CROW_LOG_WARNING << "(Archiver) page id: " << page->id()
                       << " has no associated ocr path";
      continue;
    }
    const auto pp = make_page_parser(page->file_type, page->ocr)->parse();
    for (const auto& line : *page) {
      if (line->has_img_path()) {
        wf.set_gt(line->wcor());
        // page parser starts with 0,
        // line ids start at 1.
        assert(line->id() > 0);
        pp->get(line->id() - 1).correct(wf);
        const auto img = copy_to_tmp_dir(line->img, dir);
        auto tmp = img.parent_path() / img.stem().replace_extension(".gt.txt");
        write_gt_file(*line, tmp);
      }
    }
    pp->write(get_tmp_file(page->ocr, dir));
    if (page->has_img_path()) {
      copy_to_tmp_dir(page->img, dir);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void
Archiver::write_adaptive_token_set(const Path& dir) const
{
  const auto bookid = project_->id();
  const auto path = dir / "adaptive_tokens.txt";
  std::ofstream os(path.string());
  if (not os.good()) {
    throw std::system_error(errno, std::system_category(), path.string());
  }
  tables::Types t;
  tables::Adaptivetokens a;
  auto rows = conn_.db()(
    select(t.string)
      .from(t.join(a).on(t.typid == a.typid and t.bookid == a.bookid))
      .where(t.bookid == bookid));
  for (const auto& row : rows) {
    os << row.string << "\n";
  }
  os.close();
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path
Archiver::get_tmp_file(const Path& source, const Path& tmpdir) const
{
  const auto base = remove_common_base_path(source.parent_path(), tmpdir);
  const auto target = tmpdir / base / source.filename();
  return target;
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path
Archiver::copy_to_tmp_dir(const Path& source, const Path& tmpdir) const
{
  const auto target = get_tmp_file(source, tmpdir);
  boost::system::error_code ec;
  fs::create_directories(target.parent_path(), ec);
  if (ec)
    THROW(Error,
          "(Archiver) cannot create directory: ",
          target.parent_path(),
          ": ",
          ec.message());
  CROW_LOG_DEBUG << "(Archiver) copying " << source << " to " << target;
  hard_link_or_copy(source, target, ec);
  if (ec)
    THROW(Error,
          "(Archiver) cannot copy ",
          source,
          " to ",
          target,
          ": ",
          ec.message());
  return target;
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path
Archiver::archive_name() const noexcept
{
  static const auto delre = std::regex(R"([<>:"'/\\|?*]+)");
  static const auto replre = std::regex(R"(\s+)");
  assert(project_);
  auto basename = project_->origin().data.dir.filename().string() + "_" +
                  project_->origin().data.author + "_" +
                  project_->origin().data.title + "_" +
                  std::to_string(project_->origin().data.year) + ".zip";
  basename = std::regex_replace(basename, delre, "");
  basename = std::regex_replace(basename, replre, "_");
  return basename;
}

////////////////////////////////////////////////////////////////////////////////
Path
Archiver::remove_common_base_path(const Path& p, const Path& base)
{
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
