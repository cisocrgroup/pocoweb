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
	void zip(const Path& dir, const Path& archive) const;
	void copy_files(const Path& dir) const;
	void write_gt_file(const Line& line, const Path& to) const;
	Path archive_name() const noexcept;
	static Path copy_to_tmp_dir(const Path& source, const Path& tmpdir);
	static Path remove_common_base_path(const Path& p, const Path& base);
	static void copy(const Path& from, const Path& to);

	const std::shared_ptr<const Project> project_;
	const bool write_gt_files_;
};
}
#endif  // pcw_Archiver_hpp__
