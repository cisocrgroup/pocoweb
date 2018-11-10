#include "SplitImagesRoute.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/SessionDirectory.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <crow.h>
#include <regex>
#include <unicode/uchar.h>
#include <utf8.h>

#define SPLIT_IMAGES_ROUTE_ROUTE                                               \
  "/books/<int>/pages/<int>/lines/<int>/tokens/<int>/split-images"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* SplitImagesRoute::route_ = SPLIT_IMAGES_ROUTE_ROUTE;
const char* SplitImagesRoute::name_ = "SplitImagesRoute";

////////////////////////////////////////////////////////////////////////////////
void
SplitImagesRoute::Register(App& app)
{
  CROW_ROUTE(app, SPLIT_IMAGES_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
SplitImagesRoute::impl(HttpGet,
                       const Request& req,
                       int bid,
                       int pid,
                       int lid,
                       int tid) const
{
  const auto json = crow::json::load(req.body);
  LockedSession session(must_find_session(req));
  auto conn = must_get_connection();
  const auto line = session->must_find(conn, bid, pid, lid);
  return create_split_images(*session, *line, tid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
SplitImagesRoute::create_split_images(const Session& session,
                                      const Line& line,
                                      int tokenid) const
{
  boost::optional<Token> token;
  line.each_token([&](const auto& t) {
    if (t.id == tokenid)
      token = t;
  });
  if (not token)
    THROW(NotFound, "(SplitImagesRoute) cannot find token");
  const auto split_images =
    session.directory().create_split_images(line, token->box);
  Json json;
  json["leftImg"] = nullptr;
  json["middleImg"] = nullptr;
  json["rightImg"] = nullptr;
  if (std::get<0>(split_images))
    json["leftImg"] = std::get<0>(split_images)->string();
  if (std::get<1>(split_images))
    json["middleImg"] = std::get<1>(split_images)->string();
  if (std::get<2>(split_images))
    json["rightImg"] = std::get<2>(split_images)->string();
  return json;
}
