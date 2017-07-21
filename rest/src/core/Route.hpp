#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include <crow/http_request.h>
#include <crow/http_response.h>
#include <boost/optional.hpp>
#include <memory>
#include <vector>
#include "database/mysql.hpp"
#include "utils/Error.hpp"

namespace crow {
template <typename... Middleware>
class Crow;
}

namespace pcw {
class Database;
struct Config;
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
struct AppCache;
using CachePtr = std::shared_ptr<AppCache>;
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class Page;
using PagePtr = std::shared_ptr<Page>;
class Line;
using LinePtr = std::shared_ptr<Line>;
class SessionStore;
using SessionStoreSptr = std::shared_ptr<SessionStore>;
class Route;

template <class T>
class SessionObject {
       public:
	SessionObject() = delete;
	~SessionObject() noexcept = default;
	SessionObject(const SessionObject&) = delete;
	SessionObject& operator=(const SessionObject&) = delete;
	SessionObject(SessionObject&&) = default;
	SessionObject& operator=(SessionObject&&) = default;

	Session& session() const noexcept { return *session_; }
	MysqlConnection& conn() noexcept { return conn_; }
	T& data() const noexcept { return *data_; }

       private:
	template <class F>
	SessionObject(SessionPtr s, MysqlConnection c, F f)
	    : session_(s),
	      conn_(std::move(c)),
	      data_(),
	      session_lock_(std::make_unique<std::lock_guard<Session>>(*s)) {
		data_ = f(*this);
	}
	friend class Route;
	SessionPtr session_;
	MysqlConnection conn_;
	std::shared_ptr<T> data_;
	std::unique_ptr<std::lock_guard<Session>> session_lock_;
};

class Route {
       public:
	using Response = crow::response;
	using Request = crow::request;
	using RoutingParams = crow::routing_params;
	using ProjectSessionObject = SessionObject<Project>;
	using PageSessionObject = SessionObject<Page>;
	using LineSessionObject = SessionObject<Line>;

	static Response ok() noexcept { return Response(200); }
	static Response created() noexcept { return Response(201); }
	static Response accepted() noexcept { return Response(202); }
	static Response bad_request() noexcept { return Response(400); }
	static Response forbidden() noexcept { return Response(403); }
	static Response not_found() noexcept { return Response(404); }
	static Response internal_server_error() noexcept {
		return Response(500);
	}
	static Response not_implemented() noexcept { return Response(501); }

	using App = crow::Crow<>;
	virtual ~Route() noexcept;
	virtual void Register(App&) = 0;
	virtual const char* route() const noexcept = 0;
	virtual const char* name() const noexcept = 0;
	virtual void Deregister(App&) {}
	void set_cache(CachePtr cache) noexcept { cache_ = std::move(cache); }
	void set_config(ConfigPtr c) noexcept { config_ = std::move(c); }
	void set_session_store(SessionStoreSptr s) noexcept {
		session_store_ = std::move(s);
	}
	void set_connection_pool(MysqlConnectionPoolSptr pool) noexcept {
		connection_pool_ = std::move(pool);
	}

       protected:
	SessionPtr new_session(const User& user) const;
	SessionPtr session(const Request& request) const;
	SessionPtr find_session(const Request& request) const;
	void delete_session(const Session& session) const;
	const Config& config() const noexcept { return *config_; }
	MysqlConnection connection() const noexcept {
		return connection_pool_->get_connection();
	}
	const MysqlConnectionPoolSptr& connection_pool() const noexcept {
		return connection_pool_;
	}
	ProjectSessionObject new_project_session(const Request& req,
						 int pid) const;
	PageSessionObject new_page_session(const Request& req, int pid,
					   int p) const;
	LineSessionObject new_line_session(const Request& req, int pid, int p,
					   int lid) const;

	static std::string extract_content(const crow::request& request);

       private:
	static std::string extract_multipart(const Request& req,
					     const std::string& boundary);
	static std::string extract_raw(const Request& req);

	SessionStoreSptr session_store_;
	MysqlConnectionPoolSptr connection_pool_;
	CachePtr cache_;
	ConfigPtr config_;
};

class Routes : private std::vector<RoutePtr> {
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

#endif  // pcw_Route_hpp__
