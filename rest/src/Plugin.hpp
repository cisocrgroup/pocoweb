#ifndef pcw_Plugin_hpp__
#define pcw_Plugin_hpp__

#include <dlfcn.h>
#include "Route.hpp"

namespace pcw {
	class App;

	class Plugin {
	public:
		typedef const char* (*plugin_t)(const std::string&,pcw::App&);

		Plugin(const std::string& lib)
			: dl_(nullptr)
		{
			dl_ = dlopen(lib.data(), RTLD_LAZY);
			if (not dl_)
				throw std::runtime_error(dlerror());
		}
		~Plugin() noexcept
		{
			dlclose(dl_);
		}
		void operator()(const std::string& p, App& app) const
		{
			dlerror(); // clear all errors
			auto f = reinterpret_cast<plugin_t>(dlsym(dl_, "plugin"));
			const char *error = dlerror();
			if (error)
				throw std::runtime_error(error);
			error = f(p, app);
			if (error)
				throw std::runtime_error(error);
		}
	private:
		void* dl_;
	};
}
#endif // pcw_Plugin_hpp__
