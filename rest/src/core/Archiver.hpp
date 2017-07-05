#ifndef pcw_Archiver_hpp__
#define pcw_Archiver_hpp__

#include <memory>

namespace pcw {
class Project;
class Archiver {
       public:
	Archiver(const Project& p);

       private:
	const std::shared_ptr<const Project> project_;
};
}
#endif  // pcw_Archiver_hpp__
