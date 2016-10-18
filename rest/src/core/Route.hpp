#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include <memory>
#include <vector>

namespace crow {
	template<typename... Middleware> class Crow;
	struct response;
}

namespace pcw {
	class Database;
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class Session;
	using SessionPtr = std::shared_ptr<Session>;
	class Sessions;
	using SessionsPtr = std::shared_ptr<Sessions>;
	class Route;
	using RoutePtr = std::unique_ptr<Route>;

	class Route {
	public:
		using App = crow::Crow<>;
		virtual ~Route() noexcept = default;
		virtual void Register(App&) = 0;
		virtual const char* route() const noexcept = 0;
		virtual const char* name() const noexcept = 0;
		virtual void Deregister(App&) {}
		
		SessionPtr session() const noexcept;
		const Sessions& sessions() const noexcept {return *sessions_;}
		Sessions& sessions() noexcept {return *sessions_;}
		void set_sessions(SessionsPtr s) noexcept {sessions_ = std::move(s);}
		const Config& config() const noexcept {return *config_;}
		void set_config(ConfigPtr c) noexcept {config_ = std::move(c);}
	
	private:
		SessionsPtr sessions_;	
		ConfigPtr config_;
	};

	class Routes: private std::vector<RoutePtr> {
	public:
		using Base = std::vector<RoutePtr>;
		using Base::value_type;
		using Base::push_back;
		using Base::clear;
		using Base::back;
		using Base::begin;
		using Base::end;
	};
}

#endif // pcw_Route_hpp__
