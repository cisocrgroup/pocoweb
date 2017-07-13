#ifndef pcw_SessionDirectory_hpp__
#define pcw_SessionDirectory_hpp__

#include <boost/filesystem/path.hpp>
#include <set>
#include <tuple>
#include "Pix.hpp"

namespace pcw {
using Path = boost::filesystem::path;
class Line;
class SessionDirectory {
       public:
	SessionDirectory(const std::string& sid);
	~SessionDirectory() noexcept;
	std::tuple<Path, Path, Path> create_split_images(const Line& line,
							 int x, int w);

       private:
	void init(const Path& dir) const;
	void close() const noexcept;
	Path write_split_image(int f, int t, const PIX& pix,
			       const Path& path) const;
	std::set<Path> dirs_;
	const std::string sid_;
};
}

#endif  // pcw_SessionDirectory_hpp__
