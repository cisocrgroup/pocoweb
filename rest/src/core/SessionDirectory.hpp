#ifndef pcw_SessionDirectory_hpp__
#define pcw_SessionDirectory_hpp__

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <set>
#include <tuple>
#include "Pix.hpp"

namespace pcw {
using Path = boost::filesystem::path;
class Line;
class SessionDirectory {
       public:
	using OptPath = boost::optional<Path>;
	using SplitImagePaths = std::tuple<OptPath, OptPath, OptPath>;

	SessionDirectory(const std::string& sid);
	~SessionDirectory() noexcept;
	const std::string& id() const noexcept { return sid_; }
	SplitImagePaths create_split_images(const Line& line, int x1, int x2);

       private:
	void init(const Path& dir) const;
	void close() const noexcept;
	static OptPath write_split_image(int f, int t, PIX& pix,
					 const Path& path);
	std::set<Path> dirs_;
	const std::string sid_;
};
}

#endif  // pcw_SessionDirectory_hpp__
