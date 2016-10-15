#ifndef pcw_App_hpp__
#define pcw_App_hpp__

#include "Config.hpp"
#include "Route.hpp"

namespace pcw {
	struct App {
		pcw::Config config;
		pcw::Route::App app;
		pcw::Routes routes;
	};
}

#endif // pcw_App_hpp__
