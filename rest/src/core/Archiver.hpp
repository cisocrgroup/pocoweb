#ifndef pcw_Archiver_hpp__
#define pcw_Archiver_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Project;
class Line;
class Archiver {
       public:
	using Path = boost::filesystem::path;
	Archiver(const Project& p, bool write_gt_files);
	Path operator()() const;

       private:
	std::string gather_files() const;
	void write_gt_files() const;
	void write_gt_file(const Line& line) const;
	Path get_gt_file(const Line& line) const;
	Path archive_path() const noexcept;
	const std::shared_ptr<const Project> project_;
	const bool write_gt_files_;
};
}
#endif  // pcw_Archiver_hpp__
