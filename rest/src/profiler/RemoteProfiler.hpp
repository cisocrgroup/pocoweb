#ifndef pcw_RemoteProfiler_hpp__
#define pcw_RemoteProfiler_hpp__

#include "Profiler.hpp"

namespace pcw {
class RemoteProfiler : public Profiler {
       public:
	RemoteProfiler(ConstBookSptr book, std::string url) : Profiler(std::move(book)), url_(std::move(url)) {}
	virtual ~RemoteProfiler() noexcept override = default;

       protected:
	virtual Profile do_profile() override;
	virtual std::vector<std::string> do_languages() override;
	const std::string url_;
};
}

#endif  // pcw_RemoteProfiler_hpp__
