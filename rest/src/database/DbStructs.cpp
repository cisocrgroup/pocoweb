#include "DbStructs.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include <crow/json.h>
#include <stdexcept>
#include <utf8.h>

////////////////////////////////////////////////////////////////////////////////
template <class It, class F> static void each_cor(It b, It e, F f) {
  while (b != e) {
    if (b->get_cor() > 0) {
      f(*b);
    }
    ++b;
  }
}

////////////////////////////////////////////////////////////////////////////////
template <class It, class F> static void each_ocr(It b, It e, F f) {
  while (b != e) {
    if (b->ocr > 0) {
      f(*b);
    }
    ++b;
  }
}

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
std::wstring DbSlice::wcor() const {
  std::wstring ret;
  ret.reserve(std::distance(begin, end));
  each_cor(begin, end, [&](const auto &c) { ret.push_back(c.get_cor()); });
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring DbSlice::wocr() const {
  std::wstring ret;
  ret.reserve(std::distance(begin, end));
  each_ocr(begin, end, [&](const auto &c) { ret.push_back(c.ocr); });
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
std::string DbSlice::cor() const { return utf8(wcor()); }

////////////////////////////////////////////////////////////////////////////////
std::string DbSlice::ocr() const { return utf8(wocr()); }

////////////////////////////////////////////////////////////////////////////////
std::vector<int> DbSlice::cuts() const {
  std::vector<int> cuts(std::distance(begin, end));
  auto out = cuts.begin();
  for (auto i = begin; i != end; ++i) {
    *out++ = i->cut;
  }
  return cuts;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<double> DbSlice::confs() const {
  std::vector<double> confs(std::distance(begin, end));
  auto out = confs.begin();
  for (auto i = begin; i != end; ++i) {
    *out++ = i->conf;
  }
  return confs;
}

////////////////////////////////////////////////////////////////////////////////
double DbSlice::average_conf() const {
  double sum = 0;
  double n = 0;
  each_ocr(begin, end, [&sum, &n](const auto &c) {
    ++n;
    sum += c.conf;
  });
  return n != 0 ? sum / n : 0;
}

////////////////////////////////////////////////////////////////////////////////
bool DbSlice::is_partially_corrected() const {
  return std::any_of(begin, end, [](const auto &c) { return c.is_cor(); });
}

////////////////////////////////////////////////////////////////////////////////
bool DbSlice::is_fully_corrected() const {
  return std::all_of(begin, end, [](const auto &c) { return c.is_cor(); });
}

////////////////////////////////////////////////////////////////////////////////
bool DbLine::load(MysqlConnection &mysql) {
  using namespace sqlpp;
  tables::Contents c;
  tables::Textlines l;
  tables::Projects p;
  auto stmnt = project_line_contents_view()
                   .where(p.id == this->projectid and
                          l.pageid == this->pageid and l.lineid == this->lineid)
                   .order_by(c.seq.asc());
  auto rows = mysql.db()(stmnt);
  if (rows.empty()) {
    return false;
  }
  load_from_row(rows.front(), *this);
  for (const auto &row : rows) {
    line.push_back({});
    load_from_row(row, line.back());
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::each_token(std::function<void(const DbSlice &)> f) const {
  auto nospace = [](const auto &c) { return not std::iswspace(c.get_cor()); };
  auto isspace = [](const auto &c) { return std::iswspace(c.get_cor()); };
  auto e = line.end();
  auto i = std::find_if(line.begin(), e, nospace);
  // setup slice
  DbSlice slice;
  slice.bookid = bookid;
  slice.projectid = projectid;
  slice.pageid = pageid;
  slice.lineid = lineid;
  slice.box.set_top(box.top());
  slice.box.set_bottom(box.bottom());
  slice.box.set_left(box.left());

  while (i != e) {
    // i is not space
    auto tmp = std::find_if(i + 1, e, isspace);
    // i != tmp and tmp == e or tmp is space
    slice.offset = int(std::distance(line.begin(), i));
    slice.box.set_right(int(std::prev(tmp)->cut));
    slice.begin = i;
    slice.end = tmp;
    f(slice);
    // next begin;
    i = std::find_if(tmp, e, nospace);
    // i == e or is not space
    slice.box.set_left(int(std::prev(i)->cut));
  }
}

////////////////////////////////////////////////////////////////////////////////
DbSlice DbLine::slice(int begin, int len) const {
  if (begin < 0 or len < 0 or size_t(begin + len) > line.size() or
      size_t(begin) >= line.size()) {
    throw std::logic_error("(DbLine::slice) invalid start or end index");
  }
  const auto b = line.begin() + begin;
  const auto e = b + len;
  auto left = begin == 0 ? this->box.left() : std::prev(b)->cut;
  auto right = b == e ? b->cut : std::prev(e)->cut;
  const auto box = Box(left, this->box.top(), right, this->box.bottom());
  return DbSlice{bookid, projectid, pageid, lineid, begin, b, e, box};
}

////////////////////////////////////////////////////////////////////////////////
int DbLine::tokenLength(int begin) const {
  if (begin < 0 or size_t(begin) >= line.size()) {
    throw std::logic_error("(DbLine::endOfToken) invalid start index");
  }
  const auto e =
      std::find_if(line.begin() + begin, line.end(),
                   [](const auto &c) { return std::iswspace(c.get_cor()); });
  return int(std::distance(line.begin() + begin, e));
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::begin_wagner_fischer(size_t b, size_t e) {
  // std::cerr << "DbLine::begin_wagner_fischer(" << b << "," << e << ")\n";
  e = std::min(e, line.size());
  assert(b <= line.size());
  assert(e <= line.size());
  // reset to original ocr
  std::vector<DbChar> tmp;
  tmp.reserve(line.size());
  std::copy(line.begin(), line.begin() + b, std::back_inserter(tmp));
  // remove possible insertions
  std::copy_if(line.begin() + b, line.begin() + e, std::back_inserter(tmp),
               [](const auto &c) { return c.ocr != 0; });
  // reset corrections
  std::for_each(line.begin() + b, line.begin() + e, [](auto &c) { c.cor = 0; });
  std::copy(line.begin() + e, line.end(), std::back_inserter(tmp));
  std::swap(line, tmp);
  offset_ = 0;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::set(size_t i, wchar_t c) {
  // std::cerr << "DbLine::set(" << i << "," << char(c) << ")\n";
  const auto ii = i + offset_;
  assert(ii < line.size());
  line[ii].cor = c;
  assert(line[ii].is_subst());
  assert(line[ii].is_cor());
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::insert(size_t i, wchar_t c) {
  // std::cerr << "DbLine::insert(" << i << "," << char(c) << ")\n";
  const auto ii = i + offset_;
  assert(ii <= line.size());
  const auto left = ii > 0 ? line[ii - 1].cut : box.left();
  const auto right = ii == line.size() ? box.right() : line[ii + 1].cut;
  const auto cut = (right - left) / 2;
  if (ii == line.size()) { // insertion at the end
    line.push_back(DbChar{0, c, 1, cut});
  } else {
    line.insert(line.begin() + ii, DbChar{0, c, 1, cut});
  }
  assert(ii < line.size());
  assert(line[ii].is_ins());
  assert(line[ii].is_cor());
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::erase(size_t i) {
  // std::cerr << "DbLine::erase(" << i << ")\n";
  const auto ii = i + offset_;
  assert(ii < line.size());
  line[ii].cor = DbChar::DEL;
  assert(line[ii].is_del());
  assert(line[ii].is_cor());
  ++offset_;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::noop(size_t i) {
  // std::cerr << "DbLine::noop(" << i << ")\n";
  const auto ii = i + offset_;
  assert(ii < line.size());
  // Noop means that the ocr char is correct.
  // We still need to mark it as corrected though.
  line[ii].cor = line[ii].ocr;
  line[ii].conf = 1;
  assert(line[ii].is_cor());
}

////////////////////////////////////////////////////////////////////////////////
bool DbPage::load(MysqlConnection &mysql) {
  using namespace sqlpp;
  tables::Pages pages;
  tables::Projects p;
  tables::Textlines l;
  tables::Contents c;
  auto pstmnt = project_pages_view().where(p.id == this->projectid and
                                           pages.pageid == this->pageid);
  auto lstmnt = project_line_contents_view()
                    .where(p.id == this->projectid and l.pageid == this->pageid)
                    .order_by(l.lineid.asc(), c.seq.asc());
  auto prows = mysql.db()(pstmnt);
  if (prows.empty()) {
    return false;
  }
  load_from_row(prows.front(), *this);
  int lid = -1;
  for (auto &row : mysql.db()(lstmnt)) {
    // append new line
    if (int(row.lineid) != lid) {
      this->lines.push_back(DbLine{projectid, pageid, 0});
      load_from_row(row, this->lines.back());
      lid = int(row.lineid);
    }
    // push back content to last line
    lines.back().line.push_back({});
    load_from_row(row, lines.back().line.back());
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool DbPackage::load(MysqlConnection &mysql) {
  using namespace sqlpp;
  tables::Projects p;
  tables::ProjectPages pp;
  auto rows = mysql.db()(project_book_pages_view()
                             .where(p.id == projectid)
                             .order_by(pp.pageid.asc()));
  if (not rows.empty()) {
    load_from_row(rows.front(), *this);
    for (const auto &row : rows) {
      pageids.push_back(int(row.pageid));
    }
    return true;
  }
  // book/package without pages?
  auto rowsx = mysql.db()(project_books_view().where(p.id == projectid));
  if (rowsx.empty()) {
    return false;
  }
  load_from_row(rowsx.front(), *this);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbSlice &slice) {
  j["bookId"] = slice.bookid;
  j["projectId"] = slice.projectid;
  j["pageId"] = slice.pageid;
  j["lineId"] = slice.lineid;
  j["tokenId"] = slice.offset;
  j["offset"] = slice.offset;
  j["box"] << slice.box;
  j["cor"] = slice.cor();
  j["ocr"] = slice.ocr();
  j["cuts"] = slice.cuts();
  j["confidences"] = slice.confs();
  j["averageConfidence"] = fix_double(slice.average_conf());
  j["isFullyCorrected"] = slice.is_fully_corrected();
  j["isPartiallyCorrected"] = slice.is_partially_corrected();
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbLine &line) {
  DbSlice slice{0, 0, 0, 0, 0, line.line.begin(), line.line.end()};
  j["bookId"] = line.bookid;
  j["projectId"] = line.projectid;
  j["pageId"] = line.pageid;
  j["lineId"] = line.lineid;
  j["box"] << line.box;
  j["imgFile"] = line.imagepath;
  j["cor"] = slice.cor();
  j["ocr"] = slice.ocr();
  j["cuts"] = slice.cuts();
  j["confidences"] = slice.confs();
  j["averageConfidence"] = fix_double(slice.average_conf());
  j["isFullyCorrected"] = slice.is_fully_corrected();
  j["isPartiallyCorrected"] = slice.is_partially_corrected();
  j["tokens"] = crow::json::rvalue(crow::json::type::List);
  auto i = 0;
  line.each_token([&](const auto &token) { j["tokens"][i++] << token; });
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbPage &page) {
  j["bookId"] = page.bookid;
  j["projectId"] = page.projectid;
  j["pageId"] = page.pageid;
  j["prevPageId"] = (int)page.prevpageid;
  j["nextPageId"] = (int)page.nextpageid;
  j["box"] << page.box;
  j["imgFile"] = page.imagepath;
  j["ocrFile"] = page.ocrpath;
  j["lines"] = crow::json::rvalue(crow::json::type::List);
  auto i = 0;
  for (const auto &line : page.lines) {
    j["lines"][i++] << line;
  }
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbPackage &package) {
  j["bookId"] = package.bookid;
  j["projectId"] = package.projectid;
  j["owner"] = package.owner;
  j["pageIds"] = package.pageids;
  j["pages"] = package.pageids.size();
  j["title"] = package.title;
  j["author"] = package.author;
  j["description"] = package.description;
  j["uri"] = package.uri;
  j["profilerUrl"] = package.profilerurl;
  j["histPatterns"] = package.histpatterns;
  j["directory"] = package.directory;
  j["language"] = package.language;
  j["year"] = package.year;
  j["isBook"] = bool(package.bookid == package.projectid);
  j["status"]["profiled"] = package.profiled;
  j["status"]["post-corrected"] = package.postCorrected;
  j["status"]["extended-lexicon"] = package.extendedLexicon;
  return j;
}
