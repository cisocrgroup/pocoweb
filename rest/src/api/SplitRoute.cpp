#include "SplitRoute.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>

#define SPLIT_ROUTE_ROUTE "/books/<int>/split"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *SplitRoute::route_ = SPLIT_ROUTE_ROUTE;
const char *SplitRoute::name_ = "SplitRoute";

////////////////////////////////////////////////////////////////////////////////
void SplitRoute::Register(App &app) {
  CROW_ROUTE(app, SPLIT_ROUTE_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SplitRoute::impl(HttpPost, const Request &req, int bid) const {
  CROW_LOG_INFO << "(SplitRoute) body: " << req.body;
  auto data = crow::json::load(req.body);
  const auto random = get<bool>(data, "random").value_or(false);
  const auto ids = get<std::vector<int>>(data, "ids");
  if (not ids.has_value() or ids.value().empty()) {
    THROW(BadRequest,
          "(SplitRoute) cannot split package: missing or empty ids");
  }
  auto conn = must_get_connection();
  Splitter splitter(conn, bid, random);
  return splitter.toJSON(splitter.split(ids.value()));

  // auto proj = session->must_find(conn, bid);
  // if (not proj->is_book())
  //   const auto n = data["n"].i();
  // if (n <= 0 or static_cast<size_t>(n) > proj->size())
  //   THROW(BadRequest, "cannot split book: invalid split number: ", n);
  // std::vector<ProjectPtr> projs(n);
  // std::generate(begin(projs), end(projs), [&]() {
  //   return std::make_shared<Package>(0, session->user(), proj->origin());
  // });
  // // project is a book so it's origin() points to itself
  // if (data.has("random") and data["random"].b())
  //   split_random(proj->origin(), projs);
  // else
  //   split_sequencial(proj->origin(), projs);
  // MysqlCommiter commiter(conn);
  // for (const auto &p : projs) {
  //   assert(p);
  //   insert_project(conn.db(), *p);
  // }
  // commiter.commit();
  // Json j;
  // return j << projs;
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<int, std::vector<int>>
Splitter::split(const std::vector<int> &userids) {
  using namespace sqlpp;
  tables::Projects p;
  auto rows = c_.db()(select(all_of(p)).from(p).where(p.id == pid_));
  if (rows.empty()) {
    THROW(NotFound, "(Splitter) cannot split project: not found");
  }
  if (rows.front().origin != rows.front().id) { // not a project
    THROW(BadRequest,
          "(Splitter) cannot split package: only projects can be split");
  }
  owner_ = rows.front().owner;
  const auto projs = genProjects(userids.size());
  return assign(userids, projs);
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<int, std::vector<int>>
Splitter::assign(const std::vector<int> &userids,
                 const std::vector<std::vector<int>> &projs) const {
  assert(userids.size() == projs.size());
  MysqlCommiter commiter(c_);
  using namespace sqlpp;
  tables::ProjectPages pp;
  tables::Projects p;
  auto insertp = c_.db().prepare(
      insert_into(p).set(p.origin = pid_, p.owner = parameter(p.owner),
                         p.pages = parameter(p.pages)));
  auto insertpp = c_.db().prepare(
      insert_into(pp).set(pp.projectid = parameter(pp.projectid),
                          pp.pageid = parameter(pp.pageid)));
  std::unordered_map<int, std::vector<int>> projmap;
  for (auto i = 0U; i < userids.size(); i++) {
    insertp.params.pages = static_cast<int>(projs[i].size());
    const auto pid = c_.db()(insertp);
    for (const auto pageid : projs[i]) {
      insertpp.params.projectid = pid;
      insertpp.params.pageid = pageid;
      c_.db()(insertpp);
      projmap[pid].push_back(pageid);
    }
  }
  commiter.commit();
  return projmap;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<int>> Splitter::genProjects(size_t n) const {
  using namespace sqlpp;
  tables::ProjectPages p;
  auto rows = c_.db()(select(all_of(p)).from(p).where(p.projectid == pid_));
  std::vector<int> pageids;
  for (const auto &row : rows) {
    pageids.push_back(row.pageid);
  }
  if (random_) {
    std::mt19937 g(genseed());
    std::shuffle(begin(pageids), end(pageids), g);
  }
  std::vector<std::vector<int>> projs(n);
  const auto d = pageids.size() / n;
  auto r = pageids.size() % n;
  auto o = 0U;
  for (auto i = 0U; i < n; ++i) {
    for (auto j = 0U; j < (d + r); ++j) {
      projs[i].push_back(pageids[j + o]);
    }
    o += (d + r);
    if (r > 0) {
      --r;
    }
    std::sort(projs[i].begin(), projs[i].end());
  }
  return projs;
}

////////////////////////////////////////////////////////////////////////////////
Json Splitter::toJSON(
    const std::unordered_map<int, std::vector<int>> &projs) const {
  Json j;
  return j;
}
