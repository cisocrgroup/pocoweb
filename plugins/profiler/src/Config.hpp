#ifndef profiler_Config_hpp__
#define profiler_Config_hpp__

#include <boost/filesystem/path.hpp>

namespace pcw {
	class Config;
	using Path = boost::filesystem::path;
}

namespace profiler {
	class Config {
	public:
		static void setup(const std::string& name, const pcw::Config& config);
		static const Config& get() noexcept {return *INSTANCE_;}

		const pcw::Path& backend() const noexcept {return backend_;}
		const pcw::Path& exe() const noexcept {return exe_;}
		const std::string& name() const noexcept {return name_;}
		bool local() const noexcept {return local_;}

	private:
		Config(const std::string& name, const pcw::Config& config);
		const pcw::Path backend_, exe_;
		const std::string name_;
		const bool local_;

		static std::unique_ptr<const Config> INSTANCE_;
	};
}

#endif // profiler_Config_hpp__
