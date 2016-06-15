#include <boost/log/trivial.hpp>
#include "server_http.hpp"
#include "util/Config.hpp"
#include "api.hpp"

////////////////////////////////////////////////////////////////////////////////
struct BadRequest: public pcw::Api {
	void operator()(Response& response, RequestPtr) const noexcept {
		badRequest(response, "Invalid rest uri");
	}
};

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<pcw::Api::Server>
pcw::Api::server(const Config& config)
{
	auto server = std::make_unique<Server>(config.daemon.port,
					       config.daemon.threads);
	server->resource[R"(^/login\??(.+)$)"]["GET"] = Login(config);
	server->default_resource["POST"] = BadRequest();
	server->default_resource["GET"] = BadRequest();
	return std::move(server);
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Api::reply(Status status,
		Response& response,
		const std::string& content) const noexcept
{
	switch (status) {
	case Status::Ok:
		ok(response, content);
		break;
	case Status::BadRequest:
		badRequest(response, content);
		break;
	case Status::InternalServerError:
		internalServerError(response);
		break;
	case Status::Forbidden:
		forbidden(response);
		break;
	default:
		assert(false and "Invalid status");
	}
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Api::ok(Response& response, const std::string& content) const noexcept
{
	try {
		response << "HTTP/1.1 200 OK\r\n"
			 << "Content-Type: application/json; charset=UTF-8\r\n"
			 << "Content-Length: " << content.size() << "\r\n\r\n"
			 << content << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		internalServerError(response);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Api::badRequest(Response& response, const std::string& content) const noexcept
{
	try {
		response << "HTTP/1.1 400 Bad Request\r\n"
			 << "Content-Length: " << content.size() << "\r\n\r\n"
			 << content << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		internalServerError(response);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Api::internalServerError(Response& response) const noexcept
{
	try {
		response << "HTTP/1.1 500 Internal Server Error\r\n"
			 << "Content-Length: 0\r\n\r\n"
			 << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Api::forbidden(Response& response) const noexcept
{
	try {
		response << "HTTP/1.1 403 Forbidden\r\n"
			 << "Content-Length: 0\r\n\r\n"
			 << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		internalServerError(response);
	}
}
