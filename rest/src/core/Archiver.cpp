#include "Archiver.hpp"
#include <crow/logging.h>
#include <boost/filesystem/operations.hpp>
#include <cerrno>
#include <fstream>
#include <regex>
#include <system_error>
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"

using namespace pcw;
namespace fs = boost::filesystem;

////////////////////////////////////////////////////////////////////////////////
Archiver::Archiver(const Project& project, bool write_gt_files)
    : project_(project.shared_from_this()), write_gt_files_(write_gt_files) {}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::operator()() const {
	assert(project_);
	const auto archive = project_->origin().data.dir / archive_name();
	const auto dir = project_->origin().data.dir / archive.stem();
	ScopeGuard deltmpdir([&dir]() { fs::remove_all(dir); });
	ScopeGuard delarchive([&archive]() { fs::remove(archive); });
	copy_files(dir);
	zip(dir, archive);
	delarchive.dismiss();
	// do *not* dismiss deltmpdir; it should be removed all the time.
	return archive;
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::zip(const Path& dir, const Path& archive) const {
	const auto old = fs::current_path();
	ScopeGuard restoreold([&old]() { fs::current_path(old); });
	auto new__ = dir.parent_path();
	// change into the directory to fix file paths in the resulting zip
	// archive.
	fs::current_path(new__);
	const auto command = "zip -qq -r " + archive.filename().string() + " " +
			     dir.filename().string();
	CROW_LOG_DEBUG << "(Archiver) zip command: " << command;
	const auto err = system(command.data());
	if (err)
		THROW(Error, "Cannot unzip file: `", command, "` returned ",
		      err);
	// do *not* dismiss restoreold, since we want to change back to the old
	// working directory.
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::write_gt_file(const Line& line, const Path& to) const {
	std::ofstream os(to.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(),
					to.string());
	os << line.cor() << std::endl;
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::copy_files(const Path& dir) const {
	assert(project_);
	const auto base = project_->origin().data.dir;
	for (const auto& page : *project_) {
		if (page->has_ocr_path()) {
			copy_to_tmp_dir(page->ocr, dir);
		}
		if (page->has_img_path()) {
			copy_to_tmp_dir(page->img, dir);
		}
		for (const auto& line : *page) {
			if (line->has_img_path()) {
				const auto img =
				    copy_to_tmp_dir(line->img, dir);
				if (write_gt_files_) {
					auto tmp = img;
					tmp.replace_extension(".gt.txt");
					write_gt_file(*line, tmp);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::copy_to_tmp_dir(const Path& source,
					 const Path& tmpdir) {
	const auto base = remove_common_base_path(source.parent_path(), tmpdir);
	const auto target = tmpdir / base / source.filename();
	fs::create_directories(tmpdir / base);
	copy(source, target);
	return target;
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::archive_name() const noexcept {
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
void Archiver::copy(const Path& from, const Path& to) {
	CROW_LOG_DEBUG << "(Archiver) copying " << from << " to " << to;
	boost::system::error_code ec;
	fs::create_hard_link(from, to, ec);
	if (ec) {  // could not create hard link; try copy
		CROW_LOG_WARNING << "Could not create hardlink from " << from
				 << " to " << to << ": " << ec.message();
		fs::copy_file(from, to, ec);
		if (ec) {
			THROW(Error, "(Archiver) Cannot copy ", from.string(),
			      " to ", to.string(), ": ", ec.message());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
Path Archiver::remove_common_base_path(const Path& p, const Path& base) {
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
