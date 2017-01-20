#ifndef pcw_Plugin_hpp__
#define pcw_Plugin_hpp__

#include <dlfcn.h>
#include <stdexcept>
#include "utils/Error.hpp"
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
				error(dlerror()); // noreturn
			// std::cerr << "Opened dl: " << dl_ << "\n";
		}

		~Plugin() noexcept
		{
			try {
				close();
			} catch (const std::exception& e) {
				std::cerr << "[error] Could not close plugin: "
					  << e.what() << "\n";
			}
		}

		Plugin(const Plugin&) = delete;
		Plugin& operator=(const Plugin&) = delete;
		Plugin(Plugin&& o): dl_(o.dl_) {o.dl_ = nullptr;}
		Plugin& operator=(Plugin&& o)
		{
			close();
			dl_ = o.dl_;
			o.dl_ = nullptr;
			return *this;
		}

		void operator()(const std::string& p, App& app) const
		{
			dlerror(); // clear all errors
			auto f = reinterpret_cast<plugin_t>(dlsym(dl_, "plugin"));
			const char *error = dlerror();
			if (error)
				THROW(Error, error);
			error = f(p, app);
			if (error)
				THROW(Error, error);
		}

	private:
		void close() const
		{
			// std::cerr << "Closing dl: " << dl_ << "\n";
			if (dl_) { // it is an error to close a null handle
				if (dlclose(dl_))
					error(dlerror());
			}
			// std::cerr << "Closed dl: " << dl_ << "\n";
		}
		[[noreturn]] void error(const char* e) const
		{
			if (e)
				THROW(Error, e);
			else
				THROW(Error, "??");
		}

		void* dl_;
	};
}
#endif // pcw_Plugin_hpp__
