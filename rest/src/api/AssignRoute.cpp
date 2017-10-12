#include "AssignRoute.hpp"
#include <crow.h>
#include <boost/filesystem.hpp>
#include "core/Book.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"

#define ASSIGN_ROUTE_ROUTE "/books/<int>/assign"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* AssignRoute::route_ = ASSIGN_ROUTE_ROUTE;
const char* AssignRoute::name_ = "AssignRoute";

////////////////////////////////////////////////////////////////////////////////
void AssignRoute::Register(App& app) {
	CROW_ROUTE(app, ASSIGN_ROUTE_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response AssignRoute::impl(HttpPost, const Request& req, int bid) const {
	return assign(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response AssignRoute::assign(const Request& req, int bid) const {
	CROW_LOG_DEBUG << "(AssignRoute::assign) body: " << req.body;
	const auto data = crow::json::load(req.body);
	auto conn = must_get_connection();
	const LockedSession session(must_find_session(req));
	session->assert_permission(conn, bid, Permissions::Assign);
	const auto project = session->must_find(conn, bid);
	UserPtr user = nullptr;
	const auto id = get<int>(data, "id");
	const auto name = get<std::string>(data, "name");
	if (id)
		user = session->find_user(conn, *id);
	else if (name)
		user = session->find_user(conn, *name);
	else
		THROW(BadRequest, "cannot find user: missing user information");
	if (not user) THROW(NotFound, "cannot find user: no such user");
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
