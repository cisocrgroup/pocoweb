#ifndef pcw_App_hpp__
#define pcw_App_hpp__

#include <vector>
#include <memory>
#include "Route.hpp"
#include "Plugin.hpp"

namespace pcw {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class Sessions;
	using SessionsPtr = std::shared_ptr<Sessions>;

	class App {
	public:
		App(const char *config);
		~App() noexcept;
		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = default;
		App& operator=(App&&) = default;

		void Register(RoutePtr route);
		void register_plugins();
		const Routes& routes() const noexcept {return routes_;}
		const Config& config() const noexcept {return *config_;}
		const ConfigPtr& config_ptr() const noexcept {return config_;}
		void run();

	private:
		pcw::Route::App app_;
		pcw::Routes routes_;
		std::vector<Plugin> plugins_;
		const std::shared_ptr<Config> config_;
		const std::shared_ptr<Sessions> sessions_;
	};
}

#endif // pcw_App_hpp__
