#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include <memory>
#include <boost/optional.hpp>
#include <vector>
#include <crow/http_response.h>
#include "CacheFwd.hpp"

namespace crow {
	template<typename... Middleware> class Crow;
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

	std::string get_session_id(const crow::request& request) noexcept;
	void set_session_id(crow::response& response, const std::string& sid) noexcept;

	class Route {
	public:
		static crow::response ok() noexcept {return crow::response(200);}
		static crow::response created() noexcept {return crow::response(201);}
		static crow::response accepted() noexcept {return crow::response(202);}
		static crow::response bad_request() noexcept {return crow::response(400);}
		static crow::response forbidden() noexcept {return crow::response(403);}
		static crow::response not_found() noexcept {return crow::response(404);}
		static crow::response internal_server_error() noexcept {return crow::response(500);}
		static crow::response not_implemented() noexcept {return crow::response(501);}

		using App = crow::Crow<>;
		virtual ~Route() noexcept;
		virtual void Register(App&) = 0;
		virtual const char* route() const noexcept = 0;
		virtual const char* name() const noexcept = 0;
		virtual void Deregister(App&) {}
		
		SessionPtr session(const crow::request& request) const noexcept;
		Sessions& sessions() const noexcept {return *sessions_;}
		void set_sessions(SessionsPtr s) noexcept {sessions_ = std::move(s);}
		const Config& config() const noexcept {return *config_;}
		void set_config(ConfigPtr c) noexcept {config_ = std::move(c);}
		boost::optional<Database> database(const crow::request& request) const noexcept;
		boost::optional<Database> database(SessionPtr session) const noexcept;
		void set_user_cache(UserCachePtr uc) noexcept {user_cache_ = std::move(uc);}
		UserPtr get_user(const Database& db, const std::string& name) const;
	
	private:
		UserCachePtr user_cache_;
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
