#ifndef pcw_Archiver_hpp__
#define pcw_Archiver_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Project;
class Archiver {
       public:
	Archiver(const Project& p);

       private:
	using Path = boost::filesystem::path;
	Path archive_path() const noexcept;
	const std::shared_ptr<const Project> project_;
};
}
#endif  // pcw_Archiver_hpp__
