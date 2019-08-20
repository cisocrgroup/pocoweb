#include "SessionDirectory.hpp"
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Session.hpp"
#include "core/Config.hpp"
#include "crow/json.h"
#include "jsonify.hpp"
#include <boost/filesystem/operations.hpp>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::SessionDirectory(const std::string &sid, const Config &config)
    : dirs_(), base_dir_(config.daemon.projectdir), sid_(sid) {}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::~SessionDirectory() noexcept { close(); }

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::close() const noexcept {
  for (const auto &dir : dirs_) {
    boost::system::error_code ec;
    boost::filesystem::remove_all(dir, ec);
    if (ec) {
      CROW_LOG_WARNING << "(SessionDirectory) Could not remove directory "
                       << dir << ": " << ec.message();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::SplitImagePaths
SessionDirectory::create_split_images(const Line &line, const Box &box) {
  const auto dir = line.page().book().data.dir / sid_;
  if (not dirs_.count(dir)) {
    init(base_dir_ / dir);
    dirs_.insert(dir);
  }
  if (not box.is_within(line.box))
    THROW(BadRequest, "(SessionDirectory) Invalid coordinates");
  PixPtr pix{pixRead((base_dir_ / line.img).string().data())};
  if (not pix)
    THROW(Error, "(SessionDirectory) cannot read image file: ", line.img);
  CROW_LOG_DEBUG << "(SessionDirectory) dir: " << dir;
  const auto projectid = line.page().book().id();
  const auto pageid = line.page().id();
  const auto lineid = line.id();
  const auto splitdir = dir / "split-images";
  init(base_dir_ / splitdir);
  const auto x0 = box.left() - line.box.left();
  const auto x1 = box.right() - line.box.left();
  const auto basefilename = std::to_string(projectid) + "-" +
                            std::to_string(pageid) + "-" +
                            std::to_string(lineid) + "-" + std::to_string(x0) +
                            "-" + std::to_string(x1);
  CROW_LOG_DEBUG << "(SessionDirectory) writing split image line: "
                 << line.box.left() << " " << line.box.top() << " "
                 << line.box.right() << " " << line.box.bottom()
                 << ", token: " << x0 << " " << x1;
  SplitImagePaths paths;
  std::get<0>(paths) =
      write_split_image(0, x0, *pix, splitdir / (basefilename + "-l.png"));
  std::get<1>(paths) =
      write_split_image(x0, x1, *pix, splitdir / (basefilename + "-m.png"));
  std::get<2>(paths) =
      write_split_image(x1, pix->w, *pix, splitdir / (basefilename + "-r.png"));
  return paths;
}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::OptPath
SessionDirectory::write_split_image(int f, int t, PIX &pix,
                                    const Path &path) const {
  assert(f >= 0 and f <= static_cast<int>(pix.w));
  assert(t >= 0 and f <= static_cast<int>(pix.w));
  assert(f <= t);
  if ((t - f) <= 5)
    return {};
  if (boost::filesystem::exists(path))
    return path;
  BOX box;
  box.x = f;
  box.y = 0;
  box.w = t - f;
  box.h = pix.h;
  PixPtr tmp{pixClipRectangle(&pix, &box, nullptr)};
  if (not tmp)
    THROW(Error, "(SessionDirectory) Cannot create image: ", base_dir_ / path);
  if (pixWrite((base_dir_ / path).string().data(), tmp.get(), IFF_PNG))
    THROW(Error, "(SessionDirectory) Cannot write image: ", base_dir_ / path);
  return path;
}

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::init(const Path &dir) const {
  if (not boost::filesystem::exists(dir)) {
    boost::filesystem::create_directories(dir);
  }
}

// ////////////////////////////////////////////////////////////////////////////////
// bool SessionDirectory::check_boxes(const Box& lbox, const Box& tbox) {
// 	if (lbox.left() > lbox.right()) return false;
// 	if (lbox.top() > lbox.bottom()) return false;
// 	if (tbox.left() > tbox.right()) return false;
// 	if (tbox.top() > t.box.bottom()) return false;
// 	if (tbox.left() < lbox.left() or tbox.left() > lbox.right())
// 		return false;
// 	if (tbox.right() < lbox.left() or tbox.right() > lb
// }
