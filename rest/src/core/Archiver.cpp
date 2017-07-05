#include "Archiver.hpp"
#include <regex>
#include "Book.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Archiver::Archiver(const Project& project)
    : project_(project.shared_from_this()) {}

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
