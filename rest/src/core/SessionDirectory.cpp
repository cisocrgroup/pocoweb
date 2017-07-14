#include "SessionDirectory.hpp"
#include <boost/filesystem/operations.hpp>
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Session.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::SessionDirectory(const std::string& sid)
    : dirs_(), sid_(sid) {}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::~SessionDirectory() noexcept { close(); }

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::close() const noexcept {
	for (const auto& dir : dirs_) {
		boost::system::error_code ec;
		boost::filesystem::remove_all(dir, ec);
		if (ec) {
			CROW_LOG_WARNING
			    << "(SessionDirectory) Could not remove directory "
			    << dir << ": " << ec.message();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::SplitImagePaths SessionDirectory::create_split_images(
    const Line& line, int x1, int x2) {
	const auto basedir = line.page().book().data.dir.parent_path() / sid_;
	if (not dirs_.count(basedir)) {
		init(basedir);
		dirs_.insert(basedir);
	}
	if (x1 < 0 or x1 > line.box.right() or x2 > line.box.right())
		THROW(Error, "(SessionDirectory) Invalid coordinates");
	PixPtr pix{pixRead(line.img.string().data())};
	if (not pix)
		THROW(Error, "(SessionDirectory) cannot read image file: ",
		      line.img);
	const auto projectid = line.page().book().id();
	const auto pageid = line.page().id();
	const auto lineid = line.id();
	const auto splitdir = basedir / "split-images";
	init(splitdir);

	const auto basefilename = std::to_string(projectid) + "-" +
				  std::to_string(pageid) + "-" +
				  std::to_string(lineid) + "-" +
				  std::to_string(x1) + "-" + std::to_string(x2);
	SplitImagePaths paths;
	std::get<0>(paths) = write_split_image(
	    0, x1, *pix, splitdir / (basefilename + "-l.png"));
	std::get<1>(paths) = write_split_image(
	    x1, x2, *pix, splitdir / (basefilename + "-m.png"));
	std::get<2>(paths) = write_split_image(
	    x2, pix->w, *pix, splitdir / (basefilename + "-r.png"));
	return paths;
}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::OptPath SessionDirectory::write_split_image(
    int f, int t, PIX& pix, const Path& path) {
	assert(f >= 0 and f <= static_cast<int>(pix.w));
	assert(t >= 0 and f <= static_cast<int>(pix.w));
	assert(f <= t);
	if ((t - f) <= 0) return {};
	BOX box;
	box.x = f;
	box.y = 0;
	box.w = t - f;
	box.h = pix.h;
	PixPtr tmp{pixClipRectangle(&pix, &box, nullptr)};
	if (not tmp)
		THROW(Error, "(SessionDirectory) Cannot create image: ", path);
	if (pixWrite(path.string().data(), tmp.get(), IFF_PNG))
		THROW(Error, "(SessionDirectory) Cannot write image: ", path);
	return path;
}

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::init(const Path& dir) const {
	if (not boost::filesystem::exists(dir)) {
		boost::filesystem::create_directories(dir);
	}
}
