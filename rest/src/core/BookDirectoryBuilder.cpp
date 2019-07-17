#include "BookDirectoryBuilder.hpp"
#include "Book.hpp"
#include "Config.hpp"
#include "Page.hpp"
#include "Pix.hpp"
#include "parser/llocs.hpp"
#include "util.hpp"
#include "utils/Error.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>
#include <crow/logging.h>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static BookDirectoryBuilder::Path
create_unique_bookdir_path(const Config &config) {
  BookDirectoryBuilder::Path path(config.daemon.basedir);
  while (true) {
    auto id = gensessionid(16);
    auto dir = path / config.daemon.projectdir / id;
    if (not fs::is_directory(dir)) {
      fs::create_directory(dir);
      return BookDirectoryBuilder::Path(config.daemon.projectdir) / id;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::BookDirectoryBuilder(const Config &config)
    : BookDirectoryBuilder(config.daemon.basedir,
                           create_unique_bookdir_path(config)) {
  if (not fs::is_directory(base_dir_ / dir_)) {
    fs::create_directory(base_dir_ / dir_);
  }
  assert(fs::is_directory(base_dir_ / dir_));
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::BookDirectoryBuilder(Path base, Path dir)
    : base_dir_(base), dir_(dir), tmp_dir_(".tmp"),
      line_img_dir_("line-images") {
  if (not fs::is_directory(base_dir_ / dir_)) {
    fs::create_directory(base_dir_ / dir_);
  }
  assert(fs::is_directory(base_dir_ / dir_));
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::~BookDirectoryBuilder() noexcept {
  // CROW_LOG_INFO << "(BookDirectoryBuilder) Removing tmp dir " <<
  // tmp_dir();
  // clean up tmp directory; do not throw
  boost::system::error_code ec;
  fs::remove_all(tmp_dir(), ec);
  if (ec) {
    CROW_LOG_WARNING << "(BookDirectoryBuilder) Could not remove " << tmp_dir()
                     << ": " << ec.message();
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::remove() const {
  // CROW_LOG_INFO << "(BookDirectoryBuilder) Removing directory " <<
  // dir_;
  fs::remove_all(dir_);
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::add_zip_file_path(const std::string &path) {
  const auto tdir = tmp_dir();
  fs::create_directory(tdir);
  const auto zipfile = zip_file();
  copy(path, zipfile);
  unzip();
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::unzip() {
  const auto tdir = tmp_dir();
  const auto zipfile = zip_file();
  std::string command =
      "unzip -o -qq -d" + tdir.string() + " " + zipfile.string();
  // CROW_LOG_DEBUG << "(BookDirectoryBuilder) Unzip command: " <<
  // command;
  auto err = system(command.data());
  if (err) {
    THROW(Error, "Cannot unzip file: `", command, "` returned ", err);
  }
  fs::recursive_directory_iterator i(tdir), e;
  for (; i != e; ++i) {
    add_file(*i);
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::add_zip_file_content(const std::string &content) {
  const auto tdir = tmp_dir();
  fs::create_directory(tdir);
  const auto zip = zip_file();
  // CROW_LOG_DEBUG << "(BookDirectoryBuilder) content.size(): " <<
  // content.size();
  std::ofstream os(zip.string());
  if (not os.good())
    throw std::system_error(errno, std::system_category(), zip.string());
  os << content;
  os.close();
  unzip();
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::add_file(const Path &path) { builder_.add(path); }

////////////////////////////////////////////////////////////////////////////////
BookPtr BookDirectoryBuilder::build() const {
  const auto book = builder_.build();
  assert(book);
  setup(*book);
  book->data.dir = dir_;
  return book;
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::setup(const Book &book) const {
  for (const auto &page : book) {
    assert(page);
    setup_img_and_ocr_files(*page);
    const auto pagedir = line_img_dir() / path_from_id(page->id());
    make_line_img_files(pagedir, *page);
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::setup_img_and_ocr_files(Page &page) const {
  auto do_copy = [this](const auto &path) {
    auto to = base_dir_ / dir_ / remove_common_base_path(path, tmp_dir());
    fs::create_directories(to.parent_path());
    // do not attempt to copy ocropus directories
    if (not fs::is_directory(path)) {
      copy(path, to);
    }
    return remove_common_base_path(to, base_dir_);
  };
  if (page.has_ocr_path()) {
    page.ocr = do_copy(page.ocr);
  }
  if (page.has_img_path()) {
    page.img = do_copy(page.img);
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::make_line_img_files(const Path &pagedir,
                                               Page &page) const {
  PixPtr pix;
  if (page.has_img_path()) {
    pix.reset(pixRead((base_dir_ / page.img).string().data()));
    if (not pix)
      THROW(Error, "(BookDirectoryBuilder) Cannot read img ", page.img);
  }
  for (auto &line : page) {
    if (not line->has_img_path() and not pix) {
      CROW_LOG_WARNING << "(BookDirectoryBuilder) Missing image file for: "
                       << page.ocr;
    } else if (not line->has_img_path() and pix) {
      line->img = remove_common_base_path(pagedir / path_from_id(line->id()),
                                          base_dir_);
      // we use png as single output format,
      // since it is supported by most web browsers
      // and is also the main image format that
      // ocoropus supports.
      line->img.replace_extension(".png");
      fs::create_directories(pagedir);
      write_line_img_file(pix.get(), *line);
    } else if (line->has_img_path()) {
      const auto to =
          base_dir_ / dir_ / remove_common_base_path(line->img, tmp_dir());
      fs::create_directories(to.parent_path());
      copy(line->img, to);
      // copy additional llocs files
      if (page.file_type == FileType::Llocs) {
        const auto fromllocs = get_llocs_from_png(line->img);
        const auto tollocs = get_llocs_from_png(to);
        copy(fromllocs, tollocs);
      }
      // update line image file
      line->img = remove_common_base_path(to, base_dir_);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void clip(BOX &box, const PIX &pix) {
  if (box.x + box.w > (int)pix.w) {
    int width = pix.w - box.x;
    // CROW_LOG_WARNING << "(BookDirectoryBuilder) Clipping box
    // width from "
    // << box.w << " to " << width;
    box.w = width;
  }
  if (box.y + box.h > (int)pix.h) {
    int height = pix.h - box.y;
    // CROW_LOG_WARNING << "(BookDirectoryBuilder) Clipping box
    // height from "
    // << box.h << " to " << height;
    box.h = height;
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::write_line_img_file(void *vpix,
                                               const Line &line) const {
  auto pix = (PIX *)vpix;
  assert(pix);
  // auto format = pixGetInputFormat(pix);
  BOX box;
  box.x = line.box.left();
  box.y = line.box.top();
  box.w = line.box.width();
  box.h = line.box.height();
  clip(box, *pix);
  if (box.x + box.w <= (int)pix->w and box.y + box.h <= (int)pix->h) {
    PixPtr tmp{pixClipRectangle(pix, &box, nullptr)};
    if (not tmp or
        pixWrite((base_dir_ / line.img).string().data(), tmp.get(), IFF_PNG))
      THROW(Error, "(BookDirectoryBuilder) Cannot write img ",
            base_dir_ / line.img);
  } else {
    CROW_LOG_WARNING << "Cannot write line image for " << line.cor();
  }
}

////////////////////////////////////////////////////////////////////////////////
void BookDirectoryBuilder::copy(const Path &from, const Path &to) {
  CROW_LOG_DEBUG << "(BookDirectoryBuilder) copying " << from << " to " << to;
  boost::system::error_code ec;
  fs::create_hard_link(from, to, ec);
  hard_link_or_copy(from, to, ec);
  if (ec && ec.value() != boost::system::errc::file_exists /*EEXISTS*/) {
    THROW(Error, "(BookDirectoryBuilder) cannot copy ", from.string(), " to ",
          to.string(), ": ", ec.message());
  }
}

////////////////////////////////////////////////////////////////////////////////
BookDirectoryBuilder::Path BookDirectoryBuilder::path_from_id(int id) {
  std::stringstream os;
  os << std::hex << std::setw(10) << std::setfill('0') << id;
  return os.str();
}

////////////////////////////////////////////////////////////////////////////////
Path BookDirectoryBuilder::remove_common_base_path(const Path &p,
                                                   const Path &base) {
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
