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
	const ScopeGuard deltmpdir([&dir]() { fs::remove_all(dir); });
	ScopeGuard delarchive([&archive]() { fs::remove(archive); });
	copy_files(dir);
	zip(dir, archive);
	delarchive.dismiss();
	// do *not* dismiss deltmpdir; it is removed all the time.
	return archive;
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::zip(const Path& dir, const Path& archive) const {
	const auto command =
	    "zip -qq -r " + archive.string() + " " + dir.string();
	CROW_LOG_DEBUG << "(Archiver) zip command: " << command;
	auto err = system(command.data());
	if (err)
		THROW(Error, "Cannot unzip file: `", command, "` returned ",
		      err);
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
		const auto s1 = page->ocr;
		const auto s2 = page->img;
		const auto b1 = remove_common_base_path(s1.parent_path(), base);
		const auto b2 = remove_common_base_path(s2.parent_path(), base);
		const auto t1 = dir / b1 / s1.filename();
		const auto t2 = dir / b2 / s2.filename();
		fs::create_directories(dir / b1);
		fs::create_directories(dir / b2);
		copy(s1, t1);
		copy(s2, t2);
		for (const auto& line : *page) {
			const auto sx = line->img;
			const auto bx =
			    remove_common_base_path(sx.parent_path(), base);
			const auto tx = dir / bx / sx.filename();
			fs::create_directories(dir / bx);
			copy(sx, tx);
			if (write_gt_files_) {
				auto tz = tx;
				tz.replace_extension(".gt.txt");
				write_gt_file(*line, tz);
			}
		}
	}
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
