#ifndef pcw_TmpDir_hpp__
#define pcw_TmpDir_hpp__

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "utils/TmpDir.hpp"

namespace pcw {
using Path = boost::filesystem::path;

class TmpDir {
       public:
	TmpDir() : dir_(get_tmp_dir()) {
		boost::filesystem::create_directory(dir_);
	}
	~TmpDir() noexcept {
		boost::system::error_code ec;
		boost::filesystem::remove_all(dir_, ec);
	}
	operator Path() const noexcept { return dir_; }
	Path operator/(const Path& other) const noexcept {
		return dir_ / other;
	}
	const Path& dir() const noexcept { return dir_; }

       private:
	static Path get_tmp_dir() noexcept {
		return boost::filesystem::temp_directory_path() /
		       boost::filesystem::unique_path();
	}

	Path dir_;
};
}

#endif  // pcw_TmpDir_hpp__
