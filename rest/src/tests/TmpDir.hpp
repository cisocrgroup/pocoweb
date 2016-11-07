#ifndef pcw_TmpDir_hpp__
#define pcw_TmpDir_hpp__

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace pcw {
	using Path = boost::filesystem::path;

	class TmpDir {
	public:
		TmpDir(): dir_(boost::filesystem::temp_directory_path()) {
			boost::filesystem::create_directory(dir_);
		}
		~TmpDir() noexcept {
			boost::system::error_code ec;
			boost::filesystem::remove_all(dir_, ec);
		}
		Path operator/(const Path& other) noexcept {
			return dir_ / other;
		}
		const Path& dir() const noexcept {return dir_;}

	private:
		Path dir_;
	};
}

#endif // pcw_TmpDir_hpp__
