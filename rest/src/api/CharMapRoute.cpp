#include "CharMapRoute.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/queries.hpp"
#include "database/Database.hpp"
#include <crow.h>
#include <unicode/uchar.h>
#include <utf8.h>

#define CHAR_MAP_ROUTE_ROUTE "/books/<int>/charmap"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *CharMapRoute::route_ = CHAR_MAP_ROUTE_ROUTE;
const char *CharMapRoute::name_ = "CharMapRoute";

////////////////////////////////////////////////////////////////////////////////
void CharMapRoute::Register(App &app) {
  CROW_ROUTE(app, CHAR_MAP_ROUTE_ROUTE).methods("GET"_method)(*this);
}

class CharFreqTransducer {
public:
  CharFreqTransducer() : freqs(), output(), state(0) {}
  void delta(wchar_t c) noexcept;
  std::unordered_map<std::string, size_t>
  freq_map(const std::wstring &filter) const;

private:
  enum CharType { ignore, normal, combining };
  static CharType get_type(wchar_t c) noexcept;

  std::unordered_map<std::wstring, size_t> freqs;
  std::wstring output;
  int state; // 0, 1, 2
};

////////////////////////////////////////////////////////////////////////////////
Route::Response CharMapRoute::impl(HttpGet, const Request &req, int bid) const {
  const auto filter =
      query_get<std::string>(req.url_params, "filter").value_or("");
  std::wstring wfilter;
  utf8::utf8to32(filter.begin(), filter.end(), std::back_inserter(wfilter));

  LockedSession session(get_session(req));
  auto conn = must_get_connection();
  auto book = session->must_find(conn, bid);
  bid = book->origin().id();
  CROW_LOG_DEBUG << "(CharMapRoute) book: " << bid << ", filter: " << filter;
  using namespace sqlpp;
  tables::Contents c;
  const auto stmnt = select(c.cor, c.ocr)
                         .from(c)
                         .where(c.bookid == bid)
                         .order_by(c.pageid.asc(), c.lineid.asc(), c.seq.asc());

  CharFreqTransducer t;
  for (const auto &c : conn.db()(stmnt)) {
    const auto ocr = static_cast<wchar_t>(c.ocr);
    t.delta(ocr);
  }
  const auto freqs = t.freq_map(wfilter);
  Json j;
  j["projectId"] = book->id();
  j["bookId"] = book->origin().id();
  for (const auto &kv : freqs) {
    j["charMap"][kv.first] = kv.second;
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
void CharFreqTransducer::delta(wchar_t c) noexcept {
  const auto type = get_type(c);
  switch (state) {
  case 0:
    if (type == normal) {
      state = 1;
      output.push_back(c);
    }
    break;
  case 1: // fall through
  case 2:
    switch (type) {
    case ignore:
      state = 0;
      freqs[output]++;
      output.clear();
      break;
    case normal:
      state = 1;
      freqs[output]++;
      output.clear();
      output.push_back(c);
      break;
    case combining:
      state = 2;
      output.push_back(c);
      break;
    }
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, size_t>
CharFreqTransducer::freq_map(const std::wstring &filter) const {
  std::unordered_map<std::string, size_t> res;
  std::string key;
  for (const auto &kv : freqs) {
    if (filter.find(kv.first) != std::wstring::npos) {
      continue;
    }
    key.clear();
    utf8::utf32to8(kv.first.begin(), kv.first.end(), std::back_inserter(key));
    res[key] = kv.second;
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////
CharFreqTransducer::CharType CharFreqTransducer::get_type(wchar_t c) noexcept {
  if (c == 0 or c == -1 or std::iswspace(c)) {
    return ignore;
  }
  switch (u_charType(c)) {
  case U_NON_SPACING_MARK:
    return combining;
  default:
    return normal;
  }
}
