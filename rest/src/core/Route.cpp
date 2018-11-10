#include "Route.hpp"
#include "AppCache.hpp"
#include "Book.hpp"
#include "Cache.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include "Session.hpp"
#include "SessionStore.hpp"
#include "utils/Error.hpp"
#include <chrono>
#include <crow/logging.h>
#include <ctime>
#include <regex>
#include <sstream>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
std::pair<int, std::string>
Route::get_userid(const Request& request) const noexcept
{
  // helps if this is fast, cause it is called on each request
  static const auto re =
    std::regex("[?&]userid=([0-9]+)",
               std::regex_constants::ECMAScript | std::regex_constants::icase |
                 std::regex_constants::optimize);
  // raw_url or url
  std::smatch m;
  if (not std::regex_search(request.raw_url, m, re)) {
    return std::make_pair(0, "");
  }
  CROW_LOG_DEBUG << "(Route::get_userid) userid: " << m[1];
  return std::make_pair(std::stoi(m[1]), m[1]);
}

////////////////////////////////////////////////////////////////////////////////
SessionPtr
Route::get_session(const crow::request& request) const
{
  const auto id = get_userid(request);
  if (id.first == 0) {
    return nullptr;
  }
  assert(session_store_);
  const auto session = session_store_->find_session(id.second);
  if (session == nullptr) {
    return session_store_->new_session(id.first, cache_, get_config());
  }
  return session;
}

////////////////////////////////////////////////////////////////////////////////
void
Route::delete_session(const Session& session) const
{
  assert(session_store_);
  CROW_LOG_DEBUG << "(Route) deleting session: " << session.id();
  session_store_->delete_session(session.id());
}

////////////////////////////////////////////////////////////////////////////////
MysqlConnection
Route::must_get_connection() const
{
  auto connection = get_connection();
  if (not connection)
    THROW(ServiceNotAvailable,
          "(Route) cannot find a valid database connection");
  return connection;
}

////////////////////////////////////////////////////////////////////////////////
MysqlConnection
Route::get_connection() const
{
  assert(connection_pool_);
  return connection_pool_->get_connection();
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_content(const crow::request& request)
{
  static const std::string ContentType{ "Content-Type" };
  static const std::regex BoundaryRegex{ R"(boundary=(.*);?$)" };
  // CROW_LOG_INFO << "### BODY ###\n" << request.body;

  std::smatch m;
  if (std::regex_search(
        request.get_header_value(ContentType), m, BoundaryRegex))
    return extract_multipart(request, m[1]);
  else
    return extract_raw(request);
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_multipart(const crow::request& request,
                         const std::string& boundary)
{
  CROW_LOG_INFO << "(Route) Boundary in multipart data: " << boundary;
  auto b = request.body.find("\r\n\r\n");
  if (b == std::string::npos)
    THROW(BadRequest, "(Route) Missing '\\r\\n\\r\\n' in multipart data");
  b += 4;
  auto e = request.body.rfind(boundary);
  if (b == std::string::npos)
    THROW(BadRequest, "(Route) Boundary in multipart data ", boundary);
  if (e < b)
    THROW(BadRequest, "(Route) Boundary in multipart data ", boundary);
  CROW_LOG_INFO << "(Route) b = " << b << ", e = " << e;

  return request.body.substr(b, e - b);
}

////////////////////////////////////////////////////////////////////////////////
std::string
Route::extract_raw(const crow::request& request)
{
  return request.body;
}
