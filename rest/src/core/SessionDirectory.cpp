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
    const Line& line, int x, int w) {
	const auto basedir = line.page().book().data.dir.parent_path() / sid_;
	if (not dirs_.count(basedir)) {
		init(basedir);
		dirs_.insert(basedir);
	}
	if (x < 0 or x > line.box.right() or (x + w) > line.box.right())
		THROW(Error, "(SessionDirectory) Invalid coordinates");
	PixPtr pix;
	pix.reset(pixRead(line.img.string().data()));
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
				  std::to_string(x) + "-" + std::to_string(w);
	SplitImagePaths paths;
	std::get<0>(paths) =
	    write_split_image(0, x, *pix, splitdir / (basefilename + "-l.png"));
	std::get<1>(paths) = write_split_image(
	    x, x + w, *pix, splitdir / (basefilename + "-m.png"));
	std::get<2>(paths) = write_split_image(
	    x + w, pix->w, *pix, splitdir / (basefilename + "-r.png"));
	return paths;
}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::OptPath SessionDirectory::write_split_image(
    int f, int t, const PIX& pix, const Path& path) const {
	assert(f >= 0 and f <= static_cast<int>(pix.w));
	assert(t >= 0 and f <= static_cast<int>(pix.w));
	return path;
}

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::init(const Path& dir) const {
	if (not boost::filesystem::exists(dir)) {
		boost::filesystem::create_directories(dir);
	}
}
