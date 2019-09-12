#include "SearchRoute.hpp"
#include "core/Archiver.hpp"
#include "core/Book.hpp"
#include "core/BookDirectoryBuilder.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/Searcher.hpp"
#include "core/Session.hpp"
#include "core/WagnerFischer.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>
#include <set>
#include <unordered_set>

#define NEW_SEARCH__
#define SEARCH_ROUTE_ROUTE "/books/<int>/search"

using namespace pcw;

template <class M>
static void add_matches(Json &json, const M &matches, const std::string &q,
                        bool ep);
template <class F>
static void each_package_line(MysqlConnection &mysql, int pid, F f);

struct matches {
  std::vector<std::pair<DbLine, std::unordered_set<int>>> line_matches;
  int total;
};

struct match_results {
  std::unordered_map<std::string, matches> results;
  int bookid, projectid, total, skip, max;
};

Json &operator<<(Json &j, const match_results &res) {
  j["bookId"] = res.bookid;
  j["projectId"] = res.projectid;
  j["skip"] = res.skip;
  j["max"] = res.max;
  j["total"] = res.total;

  for (const auto &m : res.results) {
    j["matches"][m.first]["total"] = m.second.total;
    int i = 0;
    for (const auto &line_match : m.second.line_matches) {
      j["matches"][m.first]["lines"][i] = m.second.total;
      j["matches"][m.first]["lines"][i]["bookId"] = line_match.first.bookid;
      j["matches"][m.first]["lines"][i]["projectId"] =
          line_match.first.projectid;
      j["matches"][m.first]["lines"][i]["pageId"] = line_match.first.pageid;
      j["matches"][m.first]["lines"][i]["lineId"] = line_match.first.lineid;
      j["matches"][m.first]["lines"][i]["box"] << line_match.first.box;
      j["matches"][m.first]["lines"][i]["imgFile"] = line_match.first.imagepath;
      auto slice = line_match.first.slice();
      j["matches"][m.first]["lines"][i]["cor"] = slice.cor();
      j["matches"][m.first]["lines"][i]["ocr"] = slice.ocr();
      j["matches"][m.first]["lines"][i]["cuts"] = slice.cuts();
      j["matches"][m.first]["lines"][i]["confidences"] = slice.confs();
      j["matches"][m.first]["lines"][i]["averageConfidence"] =
          fix_double(slice.average_conf());
      j["matches"][m.first]["lines"][i]["isFullyCorrected"] =
          slice.is_fully_corrected();
      j["matches"][m.first]["lines"][i]["isPartiallyCorrected"] =
          slice.is_partially_corrected();
      j["matches"][m.first]["lines"][i]["tokens"] =
          crow::json::rvalue(crow::json::type::List);
      int k = 0;
      line_match.first.each_token([&](auto &slice) {
        slice.match = line_match.second.count(slice.offset);
        j["matches"][m.first]["lines"][i]["tokens"][k] << slice;
        k++;
      });
      i++;
    }
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
const char *SearchRoute::route_ = SEARCH_ROUTE_ROUTE;
const char *SearchRoute::name_ = "SearchRoute";

////////////////////////////////////////////////////////////////////////////////
void SearchRoute::Register(App &app) {
  CROW_ROUTE(app, SEARCH_ROUTE_ROUTE).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>search?q=[&p=][&skip=][&max=][&q=]...
////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request &req, int bid) const {
  const auto p = get<bool>(req.url_params, "p").value_or(false);
  const auto pskip = get<int>(req.url_params, "skip").value_or(0);
  const auto pmax = get<int>(req.url_params, "max").value_or(50);
  CROW_LOG_INFO << "(SearchRoute) GET " << bid << ": skip=" << pskip
                << ", max=" << pmax << ",p=" << p;
  const auto qs = get<std::vector<std::string>>(req.url_params, "q");
  if (not qs) {
    THROW(BadRequest, "(SearchRoute) missing query parameters");
  }
  if (p) {
    return search(req, *qs, bid, pq{pskip, pmax});
  }
#ifndef NEW_SEARCH__
  return search(req, *qs, bid, tq{pskip, pmax});
#else  // NEW_SEARCH__
  auto max = pmax;
  auto skip = pskip;
  auto conn = must_get_connection();
  match_results ret;
  ret.total = 0;
  ret.projectid = bid;
  for (const auto &q : qs.value()) {
    matches m;
    m.total = 0;
    each_package_line(conn, bid, [&](const auto &line) {
      std::unordered_set<int> offsets;
      line.each_token([&](DbSlice &slice) {
        const auto cor = slice.cor();
        if (cor != q) {
          return;
        }
        offsets.insert(slice.offset);
      });
      ret.total += int(offsets.size());
      m.total += int(offsets.size());
      if (offsets.empty()) { // we did not find any matching tokens
        return;
      }
      if (skip > 0) {
        --skip;
        return;
      }
      if (max <= 0) {
        return;
      }
      --max;
      ret.bookid = line.bookid;
      ret.max = pmax;
      ret.skip = pskip;
      m.line_matches.emplace_back(line, offsets);
    });
    if (m.total > 0) {
      ret.results[q] = m;
    }
  }
  Json j;
  return j << ret;
#endif // NEW_SEARCH__
  // DbLine line(pid, pageid, lid);
  // if (not line.load(conn)) {
  //   THROW(NotFound, "(LineRoute) cannot find ", pid, ":", pageid, ":",
  //   lid);
  // }
  // Json j;
  // return j << line;

  // const auto p = get<bool>(req.url_params, "p").value_or(false);
  // const auto skip = get<int>(req.url_params, "skip").value_or(0);
  // const auto max = get<int>(req.url_params, "max").value_or(50);
  // if (not p) {
  //   return search(req, *qs, bid, tq{skip, max});
  // } else {

  // }
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request &req,
                                    const std::vector<std::string> &qs, int bid,
                                    tq x) const {
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  Json json;
  json["matches"] = crow::json::rvalue(crow::json::type::Object);
  json["projectId"] = bid;
  json["bookId"] = project->origin().id();
  json["isErrorPattern"] = false;
  json["skip"] = x.skip;
  json["max"] = x.max;
  // json["projprojectidprojectidprojectid;
  size_t totalCount = 0;
  Searcher searcher(*project, x.skip, x.max);
  for (size_t i = 0; i < qs.size() and searcher.max() > 0; i++) {
    CROW_LOG_INFO << "(SearchRoute::search) searching project id: " << bid
                  << " for query string: " << qs[i] << " (skip = " << x.skip
                  << ", max = " << x.max << ")";
    const auto matches = searcher.find(qs[i]);
    CROW_LOG_INFO << "(SearchRoute::search) found " << matches.matches.size()
                  << " matches for q='" << qs[i] << "'";
    add_matches(json, matches, qs[i], false);
    totalCount += matches.totalCount;
  }
  json["totalCount"] = totalCount;
  return json;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(const Request &req,
                                    const std::vector<std::string> &qs, int bid,
                                    pq x) const {
  const LockedSession session(get_session(req));
  auto conn = must_get_connection();
  const auto project = session->must_find(conn, bid);
  std::vector<std::string> tokens;
  for (const auto &q : qs) {
    CROW_LOG_INFO << "(SearchRoute::search) searching project id: " << bid
                  << " for error pattern: " << q << " (skip = " << x.skip
                  << ", max = " << x.max << ")";
    tables::Errorpatterns e;
    tables::Suggestions s;
    tables::Types t;
    auto rows = conn.db()(select(t.typ)
                              .from(e.join(s)
                                        .on(e.suggestionid == s.id)
                                        .join(t)
                                        .on(t.id == s.tokentypid))
                              .where(e.pattern == q));
    for (const auto &row : rows) {
      tokens.push_back(row.typ);
    }
  }
  Json json;
  json["matches"] = crow::json::rvalue(crow::json::type::Object);
  json["matches"]["totalCount"] = 0;
  json["projectId"] = bid;
  json["bookId"] = project->origin().id();
  json["isErrorPattern"] = false;
  json["skip"] = x.skip;
  json["max"] = x.max;
  size_t totalCount = 0;
  Searcher searcher(*project, x.skip, x.max);
  for (size_t i = 0; i < tokens.size() and searcher.max() > 0; i++) {
    const auto matches = searcher.find(tokens[i]);
    CROW_LOG_INFO << "(SearchRoute::search) found " << matches.matches.size()
                  << " matches for q='" << tokens[i] << "' (skip = " << x.skip
                  << ", max = " << x.max << ")";
    add_matches(json, matches, tokens[i], true);
    totalCount += matches.totalCount;
  }
  json["totalCount"] = totalCount;
  return json;
}

////////////////////////////////////////////////////////////////////////////////
template <class M>
void add_matches(Json &json, const M &matches, const std::string &q, bool ep) {
  CROW_LOG_DEBUG << "(SearchRoute::search) building response";
  size_t i = 0;
  json["matches"][q]["totalCount"] = matches.totalCount;
  for (const auto &m : matches.matches) {
    json["matches"][q][i]["line"] << *m.first;
    size_t j = 0;
    for (const auto &token : m.second) {
      json["matches"][q][i]["tokens"][j++] << token;
    }
    ++i;
  }
  CROW_LOG_DEBUG << "(SearchRoute::search) built response";
}

////////////////////////////////////////////////////////////////////////////////
template <class F>
void each_package_line(MysqlConnection &mysql, int pid, F f) {
  DbPackage package(pid);
  if (not package.load(mysql)) {
    return;
  }
  for (auto pageID : package.pageids) {
    DbPage page(pid, pageID);
    if (not page.load(mysql)) {
      continue;
    }
    for (const auto &line : page.lines) {
      f(line);
    }
  }
}
