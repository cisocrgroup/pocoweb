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
		App(App&&) = delete;
		App& operator=(App&&) = delete;

		void Register(RoutePtr route);
		void register_plugins();
		const Routes& routes() const noexcept {return routes_;}
		const Config& config() const noexcept {return *config_;}
		const ConfigPtr& config_ptr() const noexcept {return config_;}
		void run();
		void stop() noexcept;

	private:
		pcw::Routes routes_;
		std::vector<Plugin> plugins_;
		std::unique_ptr<pcw::Route::App> app_;
		const std::shared_ptr<Config> config_;
		const std::shared_ptr<Sessions> sessions_;
	};
}

#endif // pcw_App_hpp__
