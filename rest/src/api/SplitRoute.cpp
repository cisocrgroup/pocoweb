#include "SplitRoute.hpp"
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
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
const char* SplitRoute::route_ = SPLIT_ROUTE_ROUTE;
const char* SplitRoute::name_ = "SplitRoute";

////////////////////////////////////////////////////////////////////////////////
void
SplitRoute::Register(App& app)
{
  CROW_ROUTE(app, SPLIT_ROUTE_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
SplitRoute::impl(HttpPost, const Request& req, int bid) const
{
  return split(req, bid);
}

////////////////////////////////////////////////////////////////////////////////
Route::Response
SplitRoute::split(const Request& req, int bid) const
{
  CROW_LOG_DEBUG << "(SplitRoute::split) body: " << req.body;
  auto data = crow::json::load(req.body);
  auto conn = must_get_connection();
  LockedSession session(must_find_session(req));
  auto proj = session->must_find(conn, bid);
  if (not proj->is_book())
    THROW(BadRequest, "cannot split project: only books can be split");
  if (proj->owner() != session->user())
    THROW(Forbidden, "cannot split book: permission denied");
  const auto n = data["n"].i();
  if (n <= 0 or static_cast<size_t>(n) > proj->size())
    THROW(BadRequest, "cannot split book: invalid split number: ", n);
  std::vector<ProjectPtr> projs(n);
  std::generate(begin(projs), end(projs), [&]() {
    return std::make_shared<Package>(0, session->user(), proj->origin());
  });
  // project is a book so it's origin() points to itself
  if (data.has("random") and data["random"].b())
    split_random(proj->origin(), projs);
  else
    split_sequencial(proj->origin(), projs);
  MysqlCommiter commiter(conn);
  for (const auto& p : projs) {
    assert(p);
    insert_project(conn.db(), *p);
  }
  commiter.commit();
  Json j;
  return j << projs;
}

////////////////////////////////////////////////////////////////////////////////
void
SplitRoute::split_random(const Book& book, std::vector<ProjectPtr>& projs) const
{
  assert(projs.size() <= book.size());
  std::vector<PagePtr> tmp_pages(book.size());
  std::copy(begin(book), end(book), begin(tmp_pages));
  // std::random_device rd;
  std::mt19937 g(genseed());
  std::shuffle(begin(tmp_pages), end(tmp_pages), g);
  const auto n = projs.size();
  for (auto i = 0U; i < tmp_pages.size(); ++i) {
    assert(projs[i % n]);
    projs[i % n]->push_back(*tmp_pages[i]);
  }
  for (auto& p : projs) {
    assert(p);
    std::sort(begin(*p), end(*p), [](const auto& a, const auto& b) {
      assert(a);
      assert(b);
      return a->id() < b->id();
    });
  }
}

////////////////////////////////////////////////////////////////////////////////
void
SplitRoute::split_sequencial(const Book& book,
                             std::vector<ProjectPtr>& projs) const
{
  assert(projs.size() <= book.size());
  const auto n = projs.size();
  const auto d = book.size() / n;
  auto r = book.size() % n;
  std::vector<PagePtr> tmp_pages(book.size());
  std::copy(begin(book), end(book), begin(tmp_pages));
  size_t o = 0;
  for (auto i = 0U; i < n; ++i) {
    assert(projs[i]);
    for (auto j = 0U; j < (d + r); ++j) {
      projs[i]->push_back(*tmp_pages[j + o]);
    }
    o += (d + r);
    if (r > 0)
      --r;
  }
}
