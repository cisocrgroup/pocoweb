#include <regex>
#include <mutex>
#include "Config.hpp"
#include "Api.hpp"


// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::run(const Config& config)
// {
// 	Server server(config.daemon.port, config.daemon.threads);
// 	auto sessions = std::make_shared<Sessions>();
// 	server.resource[R"(^/login/username/(.+)/password/(.+)$)"]["GET"] = 
// 		Login(sessions, config);
// 	server.resource[R"(^/books(/(\d+)(/pages/(\d+)(/lines/(\d+))?)?)?$)"]["GET"] = 
// 		GetPage(sessions, config);
// 	server.default_resource["POST"] = BadRequest();
// 	server.default_resource["GET"] = BadRequest();
// 
// 	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] starting ";
// 	std::thread sthread([&server](){server.start();});
// 	sthread.join();
// 	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] stopping";
// }

// ////////////////////////////////////////////////////////////////////////////////
// std::string
// pcw::Api::getSid(RequestPtr req) const 
// {
// 	static const std::regex sid{R"(sessionid=([0-9a-fA-F]+);?)"};
// 	std::smatch m;
// 	auto cookie = req->header.find("Cookie");
// 	if (cookie != end(req->header) and std::regex_search(cookie->second, m, sid))
// 		return m[1];
// 	
// 	return {};
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::reply(Status status,
// 		Response& response,
// 		const std::string& content,
// 		const std::string& sid) const noexcept
// {
// 	switch (status) {
// 	case Status::Ok:
// 		ok(response, sid, content);
// 		break;
// 	case Status::BadRequest:
// 		badRequest(response, content);
// 		break;
// 	case Status::InternalServerError:
// 		internalServerError(response);
// 		break;
// 	case Status::Forbidden:
// 		forbidden(response);
// 		break;
// 	case Status::NotFound:
// 		notFound(response);
// 		break;
// 	default:
// 		assert(false and "Invalid status");
// 	}
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::ok(Response& response, const std::string& sid, const std::string& content) const noexcept
// {
// 	try {
// 		response << "HTTP/1.1 200 OK\r\n"
// 			 << "Content-Type: application/json; charset=UTF-8\r\n"
// 			 << "Set-Cookie: sessionid=" << sid << "; path=*;\r\n"
// 			 << "Content-Length: " << content.size() << "\r\n\r\n"
// 			 << content << std::flush;
// 	} catch (const std::exception& e) {
// 		BOOST_LOG_TRIVIAL(error) << e.what();
// 		internalServerError(response);
// 	}
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::badRequest(Response& response, const std::string& content) const noexcept
// {
// 	try {
// 		response << "HTTP/1.1 400 Bad Request\r\n"
// 			 << "Content-Length: " << content.size() << "\r\n\r\n"
// 			 << content << std::flush;
// 	} catch (const std::exception& e) {
// 		BOOST_LOG_TRIVIAL(error) << e.what();
// 		internalServerError(response);
// 	}
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::internalServerError(Response& response) const noexcept
// {
// 	try {
// 		response << "HTTP/1.1 500 Internal Server Error\r\n"
// 			 << "Content-Length: 0\r\n\r\n"
// 			 << std::flush;
// 	} catch (const std::exception& e) {
// 		BOOST_LOG_TRIVIAL(error) << e.what();
// 	}
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::forbidden(Response& response) const noexcept
// {
// 	try {
// 		response << "HTTP/1.1 403 Forbidden\r\n"
// 			 << "Content-Length: 0\r\n\r\n"
// 			 << std::flush;
// 	} catch (const std::exception& e) {
// 		BOOST_LOG_TRIVIAL(error) << e.what();
// 		internalServerError(response);
// 	}
// }
// 
// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::Api::notFound(Response& response) const noexcept
// {
// 	try {
// 		response << "HTTP/1.1 404 Not Found\r\n"
// 			 << "Content-Length: 0\r\n\r\n"
// 			 << std::flush;
// 	} catch (const std::exception& e) {
// 		BOOST_LOG_TRIVIAL(error) << e.what();
// 		internalServerError(response);
// 	}
// }
