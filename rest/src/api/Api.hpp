#ifndef pcw_Api_hpp__
#define pcw_Api_hpp__

#include <boost/optional.hpp>
#include <cassert>
#include <memory>
#include <regex>

namespace pcw {
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class Sessions;
	using SessionPtr = std::shared_ptr<Session>;
	struct Session;
	using SessionsPtr = std::shared_ptr<Sessions>;

	template<class S>
	struct Server {
		using Response = typename S::Response;
		using Request = typename S::Request;

		Server(ConfigPtr c)
			: server(
				c->daemon.port, 
				c->daemon.threads
			)
			, config(c)
			, sessions(std::make_shared<Sessions>(*c))
		{}
		void start() {server.start();}
		S server;
		const ConfigPtr config;
		const SessionsPtr sessions;
	};

	template<class S, class T> class Api {
	public:
		using Server = S;
		using Derived = T;
		using Base = Api<S, T>;
		using Response = typename Server::Response;
		using Request = typename Server::Request;
		using RequestPtr = std::shared_ptr<Request>;
		
		struct Content {
			Content(RequestPtr r, SessionPtr s)
				: os()
				, session(std::move(s))
				, req(std::move(r))
			{}
			std::stringstream os;
			SessionPtr session;
			RequestPtr req;

		};

		enum class Status: int {
			Ok = 200, 
			Created = 201,
			BadRequest = 400, 
			Forbidden = 403,
			NotFound = 404,
			InternalServerError = 500,
		};

		void operator()(Response& res, RequestPtr req) const noexcept;
		void reg(Server& server) noexcept;
		
		Sessions& sessions() const noexcept {return *sessions_;}
		void set_sessions(SessionsPtr s) noexcept {sessions_ = std::move(s);}
		const Config& config() const noexcept {return *config_;}
		void set_config(ConfigPtr c) noexcept {config_ = std::move(c);}

	private:
		const char *get_status_string(Status status) const noexcept;
		void reply(
			Response& res, 
			Status status, 
			const Content& content
		) const noexcept;
		void fail(Response& res) const noexcept;
		SessionPtr session(const Request& req) const noexcept;
		
		ConfigPtr config_;
		SessionsPtr sessions_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
void 
pcw::Api<S, T>::reg(Server& server) noexcept
{
	set_config(server.config);
	set_sessions(server.sessions);
	static_cast<const T&>(*this).do_reg(server);
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
void 
pcw::Api<S, T>::operator()(Response& res, RequestPtr req) const noexcept
{
	assert(req);
	try {
		auto session = this->session(*req);
		Content content(req, session);
		Status status;

		// lock session if it exits
		if (session) {
			std::lock_guard<std::mutex> lock(session->mutex);
			BOOST_LOG_TRIVIAL(info) << "(Api) " << *session->user 
						<< " [" << session->sid << "]";
			status = static_cast<const T&>(*this).run(content);
			reply(res, status, content);
		} else {
			status = static_cast<const T&>(*this).run(content);
			reply(res, status, content);
		}
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		fail(res); 
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
void 
pcw::Api<S, T>::reply(
	Response& res, 
	Status status, 
	const Content& content
) const noexcept {
	try {	
		const auto str = content.os.str();
		res << "HTTP/1.1 " << static_cast<int>(status) 
		    << " " << get_status_string(status) << "\r\n" 
		    << "Content-Type: application/json; charset=UTF-8\r\n";
		if (content.session) 
		    res << "Set-Cookie: sessionid=" << content.session->sid << "; path=*\r\n";
		res << "Content-Length: " << str.size() << "\r\n\r\n"
		    << str << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		fail(res);
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
void 
pcw::Api<S, T>::fail(Response& res) const noexcept
{
	try {
		res << "HTTP/1.1 " 
		    << static_cast<int>(Status::InternalServerError) << " "
		    << get_status_string(Status::InternalServerError)
		    << "\r\n\r\n" << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		fail(res);
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
const char *
pcw::Api<S, T>::get_status_string(Status status) const noexcept
{
	switch (status) {
	case Status::Ok:
		return "OK";
	case Status::Created:
		return "Created";
	case Status::BadRequest:
		return "Bad Request";
	case Status::Forbidden:
		return "Forbidden";
	case Status::NotFound:
		return "Not Found";
	case Status::InternalServerError:
		return "Internal Server Error";
	default:
		assert(false);
	}	
}

////////////////////////////////////////////////////////////////////////////////
template<class S, class T>
pcw::SessionPtr
pcw::Api<S, T>::session(const Request& req) const noexcept
{
	static const std::regex sidre{R"(sessionid=([a-z]+);?)"};
	auto r = req.header.equal_range("Cookie");
	std::smatch m;
	for (auto i = r.first; i != r.second; ++i) {
		if (std::regex_search(i->second, m, sidre)) {
			BOOST_LOG_TRIVIAL(debug) << "sid: " << m[1];
			return this->sessions().session(m[1]);
		}
	}
	return nullptr;
}

#endif // pcw_Api_hpp__

