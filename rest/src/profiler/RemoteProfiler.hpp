#ifndef pcw_RemoteProfiler_hpp__
#define pcw_RemoteProfiler_hpp__

#include "Profiler.hpp"

namespace pcw {
class RemoteProfiler : public Profiler {
       public:
	RemoteProfiler(ConstBookSptr book, std::string url)
	    : Profiler(std::move(book)), url_(std::move(url)), buffer_() {}
	virtual ~RemoteProfiler() noexcept override = default;

       protected:
	virtual Profile do_profile() override;
	virtual std::vector<std::string> do_languages() override;

       private:
	static size_t write(char *ptr, size_t size, size_t nmemb,
			    void *userdata);
	static size_t read(void *ptr, size_t size, size_t nmemb,
			   void *userdata);
	static std::vector<std::string> parse_languages(
	    const std::string &data);
	const std::string url_;
	std::string buffer_;
};
}

#endif  // pcw_RemoteProfiler_hpp__
