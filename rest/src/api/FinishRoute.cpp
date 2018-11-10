#include "FinishRoute.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>

#define FINISH_ROUTE_ROUTE "/books/<int>/finish"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* FinishRoute::route_ = FINISH_ROUTE_ROUTE;
const char* FinishRoute::name_ = "FinishRoute";

////////////////////////////////////////////////////////////////////////////////
void
FinishRoute::Register(App& app)
{
  CROW_ROUTE(app, FINISH_ROUTE_ROUTE)
    .methods("POST"_method, "GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
FinishRoute::impl(HttpPost, const Request& req, int bid) const
{
  return finish(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
FinishRoute::finish(const Request& req, int bid) const
{
  CROW_LOG_DEBUG << "(FinishRoute::finish) body: " << req.body;
  const auto data = crow::json::load(req.body);
  auto conn = must_get_connection();
  const LockedSession session(get_session(req));
  const auto project = session->must_find(conn, bid);
  const auto pentry = pcw::select_project_entry(conn.db(), bid);
  if (not pentry)
    THROW(Error, "cannot finish project: no such project id: ", bid);
  const auto oentry = pcw::select_project_entry(conn.db(), pentry->origin);
  if (not oentry)
    THROW(Error, "cannot finish project: no such project id: ", pentry->origin);
  using namespace sqlpp;
  tables::Projects p;
  MysqlCommiter commiter(conn);
  conn.db()(
    update(p).set(p.owner = oentry->owner).where(p.id == project->id()));
  project->set_owner(oentry->owner);
  commiter.commit();
  return ok();
}
