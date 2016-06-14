#include <boost/log/trivial.hpp>
#include "server_http.hpp"
#include "api.hpp"

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
		response << "HTTP/1.1 400 Bad Request\r\n\r\n"
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
		response << "HTTP/1.1 500 Bad Internal Server Error\r\n\r\n"
			 << std::flush;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
}
