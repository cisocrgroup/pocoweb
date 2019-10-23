#include "SearchRoute.hpp"
#include "core/Book.hpp"
#include "core/Package.hpp"
#include "core/Page.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/DbStructs.hpp"
#include "utils/Error.hpp"
#include "utils/ScopeGuard.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <crow.h>
#include <random>
#include <regex>
#include <set>
#include <unordered_set>

#define SEARCH_ROUTE_ROUTE "/books/<int>/search"

using namespace pcw;

struct matches {
  matches() : line_matches{}, total{0} {}
  std::vector<std::pair<DbLine, std::unordered_set<int>>> line_matches;
  int total;
};

struct match_results {
  match_results()
      : results{}, bookid{0}, projectid{0}, total{0}, skip{0}, max{0} {}
  void add(const std::string &q, const DbLine &line, int tid);
  std::unordered_map<std::string, matches> results;
  int bookid, projectid, total, skip, max;
};

////////////////////////////////////////////////////////////////////////////////
void match_results::add(const std::string &q, const DbLine &line, int tid) {
  if (results[q].line_matches.empty()) { // no results yet; just append result
    results[q].line_matches.push_back(
        std::make_pair(line, std::unordered_set<int>{tid}));
    return;
  }
  if (results[q].line_matches.back().first.lineid == line.lineid and
      results[q].line_matches.back().first.pageid == line.pageid and
      results[q].line_matches.back().first.projectid == line.projectid) {
    results[q].line_matches.back().second.insert(tid);
    return;
  }
  // match in new line
  results[q].line_matches.push_back(
      std::make_pair(line, std::unordered_set<int>{tid}));
}

////////////////////////////////////////////////////////////////////////////////
Json &operator<<(Json &j, match_results &res) {
  j["bookId"] = res.bookid;
  j["projectId"] = res.projectid;
  j["skip"] = res.skip;
  j["max"] = res.max;
  j["total"] = res.total;

  for (auto &m : res.results) {
    j["matches"][m.first]["total"] = m.second.total;
    int i = 0;
    for (auto &line_match : m.second.line_matches) {
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
template <class F>
static void each_package_line(MysqlConnection &mysql, int pid, F f);

////////////////////////////////////////////////////////////////////////////////
template <class F>
static match_results search_impl(MysqlConnection &conn, int bid, int skip,
                                 int max, F f);
////////////////////////////////////////////////////////////////////////////////
static std::wstring regex_escape(const std::wstring &str) {
  static const auto re =
      std::wregex(L"[-[\\]{}()*+?.\\^$|#\\\\]", std::regex::optimize);
  return std::regex_replace(str, re, L"\\$&");
}

////////////////////////////////////////////////////////////////////////////////
template <class T> static std::wregex make_regex(const T &qs, bool ic) {
  std::wstring pre = L"[[:punct:]]*((";
  const std::wstring suf = L"))[[:punct:]]*";
  std::wstring restr;
  if (qs.empty()) { // handle empty queries
    restr = pre + suf;
    CROW_LOG_INFO << "(make_regex) regex: " << utf8(restr) << (ic ? "/i" : "");
    return std::wregex(restr);
  }
  for (const auto &q : qs) {
    restr += pre + regex_escape(utf8(q));
    pre = std::wstring(L")|(");
  }
  restr += suf;
  CROW_LOG_INFO << "(make_regex) regex: " << utf8(restr) << (ic ? "/i" : "");
  if (ic) {
    return std::wregex(restr, std::regex::icase);
  }
  return std::wregex(restr);
}

////////////////////////////////////////////////////////////////////////////////
const char *SearchRoute::route_ = SEARCH_ROUTE_ROUTE;
const char *SearchRoute::name_ = "SearchRoute";

////////////////////////////////////////////////////////////////////////////////
void SearchRoute::Register(App &app) {
  CROW_ROUTE(app, SEARCH_ROUTE_ROUTE).methods("GET"_method)(*this);
}

enum searchType {
  stInvalid,
  stToken,
  stPattern,
  stAC,
};
////////////////////////////////////////////////////////////////////////////////
searchType search_type(const std::string &t) {
  if (t == "token") {
    return stToken;
  } else if (t == "pattern") {
    return stPattern;
  } else if (t == "ac") {
    return stAC;
  }
  return stInvalid;
}

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>search?q=[&t=][&skip=][&max=][&i=][&q=]...
////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request &req, int bid) const {
  // "token" || "pattern" || "ac"
  const auto t = get<std::string>(req.url_params, "t").value_or("token");
  const auto pskip = get<int>(req.url_params, "skip").value_or(0);
  const auto pmax = get<int>(req.url_params, "max").value_or(50);
  const auto pi = get<bool>(req.url_params, "i").value_or(false);
  CROW_LOG_INFO << "(SearchRoute) GET " << bid << ": skip=" << pskip
                << ", max=" << pmax << ", t=" << t;
  const auto qs = get<std::vector<std::string>>(req.url_params, "q");
  if (not qs) {
    THROW(BadRequest, "(SearchRoute) missing query parameter(s)");
  }
  auto conn = must_get_connection();
  CROW_LOG_INFO << "(SearchRoute) type: " << search_type(t);
  switch (search_type(t)) {
  case stToken:
    return search_token(
        conn,
        tq{.bid = bid, .max = pmax, .skip = pskip, .ic = pi, .qs = qs.value()});
  case stPattern:
    return search_pattern(
        conn, pq{.bid = bid, .max = pmax, .skip = pskip, .qs = qs.value()});
  case stAC:
    return search_ac(
        conn, ac{.bid = bid, .max = pmax, .skip = pskip, .qs = qs.value()});
  default:
    THROW(BadRequest, "(SearchRoute) invalid search type: ", t);
  }
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search_token(MysqlConnection &mysql, tq q) const {
  const auto ic = q.ic;
  const auto re = make_regex(q.qs, ic);
  auto ret =
      search_impl(mysql, q.bid, q.skip, q.max, [&](const auto &t, auto &q) {
        std::wsmatch m;
        auto cor = t.wcor();
        if (not std::regex_match(cor, m, re)) {
          return false;
        }
        if (ic) {
          q = utf8(boost::algorithm::to_lower_copy(std::wstring(m[1])));
        } else {
          q = utf8(m[1]);
        }
        // CROW_LOG_INFO << "matched: " << t.cor() << " (" << t.ocr() << ")";
        // std::for_each(t.begin, t.end, [](const auto &c) {
        //   CROW_LOG_INFO << "char: " << int(c.cor) << ", " << int(c.ocr);
        // });
        return true;
      });
  Json j;
  return j << ret;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search_pattern(MysqlConnection &mysql,
                                            pq q) const {
  std::unordered_map<std::string, std::string> q2p; // map queries to patterns
  std::vector<std::string> qs; // list of queries with the given pattern
  DbPackage package(q.bid);
  if (not package.load(mysql)) {
    THROW(Error, "cannot load package: ", package.strID());
  }
  // select tokens
  for (const auto &p : q.qs) {
    tables::Errorpatterns e;
    tables::Suggestions s;
    tables::Types t;
    auto rows =
        mysql.db()(select(t.typ, e.pattern)
                       .from(e.join(s)
                                 .on(e.suggestionid == s.id)
                                 .join(t)
                                 .on(t.id == s.tokentypid))
                       .where(e.pattern == p and s.bookid == package.bookid and
                              s.topsuggestion == true));

    for (const auto &row : rows) {
      // skip collate artifacts
      if (row.pattern != p) {
        continue;
      }
      // later patterns overwrite prior ones
      q2p.emplace(row.typ, p);
      qs.push_back(row.typ);
    }
  }
  const auto re = make_regex(qs, true); // types and patterns are all lower case
  // search
  auto ret =
      search_impl(mysql, q.bid, q.skip, q.max, [&](const auto &t, auto &q) {
        const auto cor = t.wcor();
        std::wsmatch m;
        if (not std::regex_match(cor, m, re)) {
          return false;
        }
        q = q2p[utf8(boost::algorithm::to_lower_copy(
            std::wstring(m[1])))]; // give pattern (not matched type) as key
        return true;
      });
  Json j;
  return j << ret;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search_ac(MysqlConnection &mysql, ac q) const {
  DbPackage pkg(q.bid);
  if (not pkg.load(mysql)) {
    THROW(Error, "cannot load package ", pkg.strID());
  }
  std::unordered_set<int> pages(pkg.pageids.begin(), pkg.pageids.end());
  using namespace sqlpp;
  tables::Autocorrections acs;
  tables::Types tps;
  match_results ret;
  ret.projectid = pkg.projectid;
  ret.bookid = pkg.bookid;
  ret.max = q.max;
  ret.skip = q.skip;
  for (const auto &qstr : q.qs) {
    auto rows = mysql.db()(select(acs.pageid, acs.lineid, acs.tokenid, tps.typ)
                               .from(acs.join(tps).on(acs.ocrtypid == tps.id))
                               .where(tps.typ == qstr));
    DbLine line(q.bid, -1, -1);
    for (const auto &row : rows) {
      if (pages.count(row.pageid) == 0) { // check if page is in package
        continue;
      }
      ret.total++;
      ret.results[qstr].total++;
      if (q.skip > 0) { // skip match
        --q.skip;
        continue;
      }
      if (q.max <= 0) { // max matches found
        continue;
      }
      --q.max;
      if (line.pageid != row.pageid or
          line.lineid != row.lineid) { // load new line
        line.line.clear();
        line.lineid = row.lineid;
        line.pageid = row.pageid;
        if (not line.load(mysql)) {
          THROW(Error, "cannot line page ", line.strID());
        }
      }
      ret.add(qstr, line, row.tokenid);
    }
  }
  Json j;
  return j << ret;
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
    for (auto &line : page.lines) {
      f(line);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
template <class F>
static match_results search_impl(MysqlConnection &conn, int bid, int skip,
                                 int max, F f) {
  match_results ret;
  ret.projectid = bid;
  ret.max = max;
  ret.skip = skip;
  each_package_line(conn, bid, [&](auto &line) {
    if (line.slice().is_fully_corrected()) { // skip fully corrected lines
      return;
    }
    line.each_token([&](DbSlice &slice) {
      if (slice.is_fully_corrected()) { // skip fully corrected tokens
        return;
      }
      std::string q;         // match key
      if (not f(slice, q)) { // no match
        return;
      }
      // we have a search match
      ret.total++;
      ret.results[q].total++;
      if (skip > 0) { // skip match
        --skip;
        return;
      }
      if (max <= 0) { // max matches found
        return;
      }
      --max;
      ret.add(q, line, slice.offset);
      // set bookid from line
      ret.bookid = line.bookid;
    });
  });
  return ret;
}
