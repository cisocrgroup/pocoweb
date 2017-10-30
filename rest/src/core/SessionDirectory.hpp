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
class Box;
class Config;
class SessionDirectory {
       public:
	using OptPath = boost::optional<Path>;
	using SplitImagePaths = std::tuple<OptPath, OptPath, OptPath>;

	SessionDirectory(const std::string& sid, const Config& config);
	~SessionDirectory() noexcept;
	const std::string& id() const noexcept { return sid_; }
	SplitImagePaths create_split_images(const Line& line, const Box& b);

       private:
	void init(const Path& dir) const;
	void close() const noexcept;
	static bool check_boxes(const Box& lbox, const Box& tbox);
	OptPath write_split_image(int f, int t, PIX& pix,
				  const Path& path) const;
	std::set<Path> dirs_;
	const Path base_dir_;
	const std::string sid_;
};
}

#endif  // pcw_SessionDirectory_hpp__
