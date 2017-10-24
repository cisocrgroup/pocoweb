#ifndef PCW_REMOTE_PROFILER_TEMPLATE_HPP__
#define PCW_REMOTE_PROFILER_TEMPLATE_HPP__

#include <string>
namespace pcw {
class RemoteProfilerTemplate {
       public:
	RemoteProfilerTemplate() : data_() {}
	RemoteProfilerTemplate& set_userid(const std::string& userid);
	RemoteProfilerTemplate& set_filename(const std::string& filename);
	RemoteProfilerTemplate& set_language(const std::string& language);
	RemoteProfilerTemplate& set_data(const std::string& data);
	const std::string& get() const { return data_; }

       private:
	RemoteProfilerTemplate& set_string(const char* key, size_t n,
					   const std::string& str);
	std::string data_;
	static const char* t;
};
}
#endif  // PCW_REMOTE_PROFILER_TEMPLATE_HPP__
