#ifndef pcw_SessionDirectory_hpp__
#define pcw_SessionDirectory_hpp__

#include <boost/filesystem/path.hpp>
#include <set>

namespace pcw {
using Path = boost::filesystem::path;
class SessionDirectory {
       public:
	SessionDirectory(const std::string& sid);
	~SessionDirectory() noexcept;

       private:
	void init(const Path& dir) const;
	void close() const noexcept;
	std::set<Path> dirs_;
	const std::string sid_;
};
}

#endif  // pcw_SessionDirectory_hpp__
