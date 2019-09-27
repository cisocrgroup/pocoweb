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
  std::unordered_map<std::string, matches> results;
  int bookid, projectid, total, skip, max;
};

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

template <class F>
static void each_package_line(MysqlConnection &mysql, int pid, F f);

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
  std::wstring restr;
  for (const auto &q : qs) {
    restr += pre + regex_escape(utf8(q));
    pre = std::wstring(L")|(");
  }
  restr += L"))[[:punct:]]*";
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

////////////////////////////////////////////////////////////////////////////////
// GET /books/<bid>search?q=[&p=][&skip=][&max=][&q=]...
////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::impl(HttpGet, const Request &req, int bid) const {
  const auto p = get<bool>(req.url_params, "p").value_or(false);
  const auto pskip = get<int>(req.url_params, "skip").value_or(0);
  const auto pmax = get<int>(req.url_params, "max").value_or(50);
  const auto pi = get<bool>(req.url_params, "i").value_or(false);
  CROW_LOG_INFO << "(SearchRoute) GET " << bid << ": skip=" << pskip
                << ", max=" << pmax << ",p=" << p;
  const auto qs = get<std::vector<std::string>>(req.url_params, "q");
  if (not qs) {
    THROW(BadRequest, "(SearchRoute) missing query parameters");
  }
  auto conn = must_get_connection();
  if (p) {
    return search(conn,
                  pq{.bid = bid, .max = pmax, .skip = pskip, .qs = qs.value()});
  }
  return search(
      conn,
      tq{.bid = bid, .max = pmax, .skip = pskip, .ic = pi, .qs = qs.value()});
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
    line.each_token([&](DbSlice &slice) {
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
      // add line if first match in this line or for this q
      if (ret.results[q].line_matches.empty() or
          ret.results[q].line_matches.back().first != line) {
        ret.results[q].line_matches.emplace_back(line,
                                                 std::unordered_set<int>{});
      }
      // add token match
      ret.results[q].line_matches.back().second.insert(slice.offset);
      // set bookid from line
      ret.bookid = line.bookid;
    });
  });
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
Route::Response SearchRoute::search(MysqlConnection &mysql, tq q) const {
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
Route::Response SearchRoute::search(MysqlConnection &mysql, pq q) const {
  std::unordered_map<std::string, std::string> q2p; // map queries to patterns
  std::vector<std::string> qs; // list of queries with the given pattern
  // select tokens
  for (const auto &p : q.qs) {
    tables::Errorpatterns e;
    tables::Suggestions s;
    tables::Types t;
    auto rows = mysql.db()(select(t.typ, e.pattern)
                               .from(e.join(s)
                                         .on(e.suggestionid == s.id)
                                         .join(t)
                                         .on(t.id == s.tokentypid))
                               .where(e.pattern == p and s.bookid == q.bid and
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
