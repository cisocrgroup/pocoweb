#include "AssignRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include "core/Book.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/jsonify.hpp"
#include "core/queries.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"

#define ASSIGN_ROUTE_ROUTE "/books/<int>/assign"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* AssignRoute::route_ = ASSIGN_ROUTE_ROUTE;
const char* AssignRoute::name_ = "AssignRoute";

////////////////////////////////////////////////////////////////////////////////
void AssignRoute::Register(App& app) {
	CROW_ROUTE(app, ASSIGN_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response AssignRoute::impl(HttpGet, const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(AssignRoute::assign) body: " << req.body;
	const auto data = crow::json::load(req.body);
	auto conn = must_get_connection();
	const LockedSession session(must_find_session(req));
	session->assert_permission(conn, bid, Permissions::Assign);
	const auto project = session->must_find(conn, bid);
	const auto id = query_get<int>(req.url_params, "uid");
	if (not id) {
		THROW(BadRequest, "missing or invalid user id");
	}
	const auto user = session->find_user(conn, *id);
	if (not user) {
		THROW(NotFound, "cannot find user: no such user: ", *id);
	}
	MysqlCommiter commiter(conn);
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = update(projects)
			 .set(projects.owner = user->id())
			 .where(projects.projectid == project->id());
	conn.db()(stmnt);
	project->set_owner(*user);
	commiter.commit();
	return ok();
}
