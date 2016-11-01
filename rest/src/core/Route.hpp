#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include <memory>
#include <boost/optional.hpp>
#include <vector>
#include <crow/http_response.h>
#include <crow/http_request.h>

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
	class User;
	using UserPtr = std::shared_ptr<User>;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;
	class AppCache;
	using CachePtr = std::shared_ptr<AppCache>;

	std::string get_session_id(const crow::request& request) noexcept;
	void set_session_id(crow::response& response, const std::string& sid) noexcept;

	class Route {
	public:
		using Response = crow::response;
		using Request = crow::request;

		static Response ok() noexcept {return Response(200);}
		static Response created() noexcept {return Response(201);}
		static Response accepted() noexcept {return Response(202);}
		static Response bad_request() noexcept {return Response(400);}
		static Response forbidden() noexcept {return Response(403);}
		static Response not_found() noexcept {return Response(404);}
		static Response internal_server_error() noexcept {return Response(500);}
		static Response not_implemented() noexcept {return Response(501);}

		using App = crow::Crow<>;
		virtual ~Route() noexcept;
		virtual void Register(App&) = 0;
		virtual const char* route() const noexcept = 0;
		virtual const char* name() const noexcept = 0;
		virtual void Deregister(App&) {}
		void set_cache(CachePtr cache) noexcept {cache_ = std::move(cache);}
		void set_config(ConfigPtr c) noexcept {config_ = std::move(c);}
		void set_sessions(SessionsPtr s) noexcept {sessions_ = std::move(s);}
		
	protected:
		SessionPtr session(const Request& request) const noexcept;
		Sessions& sessions() const noexcept {return *sessions_;}
		const Config& config() const noexcept {return *config_;}
		boost::optional<Database> database(const Request& request) const noexcept;
		boost::optional<Database> database(SessionPtr session) const noexcept;
		static std::string extract_content(const crow::request& request);

	private:
		static std::string extract_multipart(
			const Request& req, 
			const std::string& boundary
		);
		static std::string extract_raw(const Request& req);
	
		SessionsPtr sessions_;
		CachePtr cache_;
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
