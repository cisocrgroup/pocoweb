#include "DbStructs.hpp"
#include "core/WagnerFischer.hpp"
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
DbSlice::DbSlice(const DbLine &line, std::list<DbChar>::const_iterator b,
                 std::list<DbChar>::const_iterator e)
    : bookid(line.bookid), pageid(line.pageid), lineid(line.lineid),
      offset(int(std::distance(line.line.cbegin(), b))), begin(b), end(e),
      box(), match(), line_(line) {
  this->box.set_top(line_.box.top());
  this->box.set_bottom(line_.box.bottom());
  if (begin == line_.line.begin()) {
    this->box.set_left(line_.box.left());
  } else {
    this->box.set_left(std::prev(begin)->cut);
  }
  if (end == begin) {
    if (end == line_.line.end()) {
      this->box.set_right(line_.box.right());
    } else {
      this->box.set_right(end->cut);
    }
  } else {
    this->box.set_right(std::prev(end)->cut);
  }
}

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
void DbLine::each_token(std::function<void(DbSlice &)> f) const {
  auto nospace = [](const auto &c) { return not std::iswspace(c.get_cor()); };
  auto isspace = [](const auto &c) { return std::iswspace(c.get_cor()); };
  auto e = line.end();
  auto i = std::find_if(line.begin(), e, nospace);
  while (i != e) {
    // i is not space
    auto tmp = std::find_if(std::next(i), e, isspace);
    // i != tmp and tmp == e or tmp is space
    DbSlice slice(*this, i, tmp);
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
      size_t(begin) > line.size()) {
    throw std::logic_error("(DbLine::slice) invalid start or len: " +
                           std::to_string(begin) + ", " + std::to_string(len) +
                           " [size = " + std::to_string(line.size()) + "]");
  }
  return DbSlice(*this, std::next(line.begin(), begin),
                 std::next(line.begin(), begin + len));
}

////////////////////////////////////////////////////////////////////////////////
int DbLine::tokenLength(int begin) const {
  if (begin < 0 or size_t(begin) >= line.size()) {
    throw std::logic_error(
        "(DbLine::endOfToken) invalid start index: " + std::to_string(begin) +
        " [size = " + std::to_string(line.size()) + "]");
  }
  const auto e =
      std::find_if(std::next(line.begin(), begin), line.end(),
                   [](const auto &c) { return std::iswspace(c.get_cor()); });
  return int(std::distance(std::next(line.begin(), begin), e));
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::begin_wagner_fischer(size_t b, size_t e) {
  std::cerr << "DbLine::begin_wagner_fischer(" << b << "," << e << ")\n";
  begin_ = std::next(line.begin(), b);
  end_ = std::next(line.begin(), e);
  for (auto i = begin_; i != end_;) {
    if (i->ocr == 0) { // delete insertions
      auto j = std::next(i);
      line.erase(i);
      i = j;
    } else { // mark as not corrected
      i->cor = 0;
      i = std::next(i);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::set(size_t i, wchar_t c) {
  std::cerr << "DbLine::set(" << i << "," << char(c) << ")\n";
  assert(begin_ != end_);
  begin_->cor = c;
  assert(begin_->is_subst());
  assert(begin_->is_cor());
  ++begin_;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::insert(size_t i, wchar_t c) {
  std::cerr << "DbLine::insert(" << i << "," << char(c) << ")\n";
  const auto left = i > 0 ? std::prev(begin_)->cut : box.left();
  const auto right = begin_ == end_ ? box.right() : begin_->cut;
  const auto cut = left + ((right - left) / 2);
  std::cerr << "left cut: " << left << ", right cut: " << right
            << ", cut: " << cut << "\n";
  line.insert(begin_, DbChar{0, c, 1, cut});
  // do not increment begin_;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::erase(size_t i) {
  std::cerr << "DbLine::erase(" << i << ")\n";
  auto next = std::next(begin_);
  line.erase(begin_);
  begin_ = next;
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::noop(size_t i) {
  std::cerr << "DbLine::noop(" << i << ")\n";
  // Noop means that the ocr char is correct.
  // We still need to mark it as corrected though.
  begin_->cor = begin_->ocr;
  // TODO: must we really set conf to 1?  This way the original ocr confidence
  // information is lost.
  begin_->conf = 1;
  std::cerr << "begin_->ocr      = " << int(begin_->ocr) << "\n";
  std::cerr << "begin_->cor      = " << int(begin_->cor) << "\n";
  std::cerr << "begin_->is_cor() = " << begin_->is_cor() << "\n";
  assert(begin_->ocr == 0 or begin_->is_cor());
  ++begin_;
}
////////////////////////////////////////////////////////////////////////////////
int DbLine::correct(WagnerFischer &wf, const std::wstring &cor) {
  // auto slice = this->slice();
  // wf.set_ocr(slice.wocr());
  // wf.set_gt(cor);
  return correct(wf, cor, 0, line.size());
}

////////////////////////////////////////////////////////////////////////////////
int DbLine::correct(WagnerFischer &wf, const std::wstring &cor, size_t b,
                    size_t len) {
  auto slice = this->slice(b, len);
  wf.set_ocr(slice.wocr());
  wf.set_gt(cor);
  auto ret = wf(b, b + len);
  wf.correct(*this, b, cor.size());
  return ret;
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
  j["match"] = slice.match;
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbLine &line) {
  const auto slice = line.slice();
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
