#ifndef pcw_App_hpp__
#define pcw_App_hpp__

#include <vector>
#include <memory>
#include "Route.hpp"
#include "Plugin.hpp"
#include "database/mysql.hpp"

namespace pcw {
	struct Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class SessionStore;
	using SessionsStoreSptr = std::shared_ptr<SessionStore>;
	struct AppCache;
	using CachePtr = std::shared_ptr<AppCache>;

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
		MysqlConnectionPool& connection_pool() const noexcept {return *connection_pool_;}
		void run();
		void stop() noexcept;
		static int version() noexcept;
		static int version_major() noexcept;
		static int version_minor() noexcept;
		static int version_patch() noexcept;
		static const char* version_str() noexcept;

	private:
		void log(const Route& route) const;

		pcw::Routes routes_;
		std::vector<Plugin> plugins_;
		std::unique_ptr<pcw::Route::App> app_;
		const CachePtr cache_;
		const std::shared_ptr<Config> config_;
		const SessionStoreSptr session_store_;
		const MysqlConnectionPoolSptr connection_pool_;
	};
}

#endif // pcw_App_hpp__
