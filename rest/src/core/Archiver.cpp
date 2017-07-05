#include "Archiver.hpp"
#include <crow/logging.h>
#include <cerrno>
#include <fstream>
#include <regex>
#include <system_error>
#include "Book.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Archiver::Archiver(const Project& project, bool write_gt_files)
    : project_(project.shared_from_this()), write_gt_files_(write_gt_files) {}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::operator()() const {
	assert(project_);
	const auto ar = archive_path();
	if (write_gt_files_) {
		write_gt_files();
	}
	const auto files = gather_files();
	const auto command = "zip -qq " + ar.string() + " " + files;
	CROW_LOG_DEBUG << "(Archiver) zip command: " << command;
	auto err = system(command.data());
	if (err)
		THROW(Error, "Cannot unzip file: `", command, "` returned ",
		      err);
	return ar;
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::write_gt_files() const {
	assert(project_);
	for (const auto& page : *project_) {
		for (const auto& line : *page) {
			write_gt_file(*line);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void Archiver::write_gt_file(const Line& line) const {
	const auto gt_file = get_gt_file(line);
	std::ofstream os(gt_file.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(),
					gt_file.string());
	os << line.cor() << std::endl;
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
std::string Archiver::gather_files() const {
	assert(project_);
	std::stringstream files;
	for (const auto& page : *project_) {
		files << page->ocr << " " << page->img << " ";
		for (const auto& line : *page) {
			files << line->img << " ";
			if (write_gt_files_)
				files << get_gt_file(*line).string() << " ";
		}
	}
	return files.str();
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::get_gt_file(const Line& line) const {
	auto gt_file = line.img;
	gt_file.replace_extension(".gt.txt");
	return gt_file;
}

////////////////////////////////////////////////////////////////////////////////
Archiver::Path Archiver::archive_path() const noexcept {
	assert(project_);
	static const auto delre = std::regex(R"([<>:"'/\\|?*]+)");
	static const auto replre = std::regex(R"(\s+)");
	auto basename = project_->origin().data.author + "_" +
			project_->origin().data.title +
			std::to_string(project_->origin().data.year) + ".zip";
	basename = std::regex_replace(basename, delre, "");
	basename = std::regex_replace(basename, replre, "_");
	return project_->origin().data.dir / basename;
}
