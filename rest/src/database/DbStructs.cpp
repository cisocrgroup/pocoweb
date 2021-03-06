#include "DbStructs.hpp"
#include "common.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
#include <crow/json.h>
#include <rapidjson/writer.h>
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
DbSlice::DbSlice(DbLine &line, iterator b, iterator e)
    : bookid(line.bookid), projectid(line.projectid), pageid(line.pageid),
      lineid(line.lineid), begin(b), end(e), box(), match(), line_(&line) {
  this->box.set_top(line_->box.top());
  this->box.set_bottom(line_->box.bottom());
  if (begin == line_->line.begin()) {
    this->box.set_left(line_->box.left());
  } else {
    this->box.set_left(std::prev(begin)->cut);
  }
  if (end == begin) {
    if (end == line_->line.end()) {
      this->box.set_right(line_->box.right());
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
int DbSlice::tokenid() const {
  return begin == line_->line.end() ? -1 : begin->id;
}

////////////////////////////////////////////////////////////////////////////////
int DbSlice::offset() const {
  return std::distance(line_->line.begin(), begin);
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
bool DbSlice::is_automatically_corrected() const {
  return std::all_of(
      begin, end, [](const auto &c) { return c.is_cor() and not c.manually; });
}

////////////////////////////////////////////////////////////////////////////////
bool DbSlice::is_manually_corrected() const {
  return std::all_of(begin, end,
                     [](const auto &c) { return c.is_cor() and c.manually; });
}

////////////////////////////////////////////////////////////////////////////////
bool DbSlice::contains_manual_corrections() const {
  return std::any_of(begin, end, [](const auto &c) { return c.manually; });
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::reset() {
  // ID represents both the seq (offset) and the id of the original
  // ocr chars.
  int id = offset();
  for (auto i = begin; i != end;) {
    auto next = std::next(i);
    if (i->is_ins()) {
      // There is no need to update the id since we remain on the same
      // offset after the deletion.
      line_->line.erase(i);
      if (i == begin) {
        begin = next;
      }
    } else {
      i->cor = 0;
      i->id = id++;
    }
    i = next;
  }
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::begin_wagner_fischer(size_t b, size_t e) {
  // We ignore b and e since we simply can correct whole (sub) slice.
  reset();
  i_ = begin;
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::set(size_t i, wchar_t c) {
  // std::cerr << "DbSlice::set(" << i << "," << char(c) << ")\n";
  assert(i_ != end);
  i_->cor = c;
  assert(i_->is_subst());
  assert(i_->is_cor());
  ++i_;
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::insert(size_t i, wchar_t c) {
  // std::cerr << "DbSlice::insert(" << i << "," << char(c) << ")\n";
  const auto left = i_ != begin ? std::prev(i_)->cut : box.left();
  const auto right = i_ == end ? box.right() : i_->cut;
  const auto cut = left + ((right - left) / 2);
  // std::cerr << "left cut: " << left << ", right cut: " << right
  //           << ", cut: " << cut << "\n";
  const auto id = int(line_->line.size());
  line_->line.insert(
      i_, DbChar{.ocr = 0, .cor = c, .conf = 1, .cut = cut, .id = id});
  if (i_ == begin) { // Fix begin if we have an insertion before start pos.
    begin = std::prev(i_);
  }
  // do not increment i_;
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::erase(size_t i) {
  // std::cerr << "DbSlice::erase(" << i << ")\n";
  assert(i_ != end);
  i_->cor = DbChar::DEL; // mark as deleted
  ++i_;
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::noop(size_t i) {
  // std::cerr << "DbSlice::noop(" << i << ")\n";
  // Noop means that the ocr char is correct.
  // We still need to mark it as corrected though.
  i_->cor = i_->ocr;
  assert(i_->is_cor());
  ++i_;
}

////////////////////////////////////////////////////////////////////////////////
void DbSlice::set_correction_type(bool manual) {
  for (auto i = begin; i != end; i++) {
    i->manually = manual;
  }
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
void DbLine::each_token(std::function<void(DbSlice &)> f) {
  auto isboundary = [](const auto &c) { return std::iswspace(c.get_cor()); };
  enum { start, bound, rune } state = start;
  const auto end = line.end();
  iterator begin;
  for (auto i = line.begin(); i != end; i++) {
    switch (state) {
    case start:
      if (isboundary(*i)) {
        state = bound;
      } else {
        begin = i;
        state = rune;
      }
      break;
    case rune:
      if (isboundary(*i)) {
        DbSlice slice(*this, begin, i);
        f(slice);
        state = bound;
      }
      break;
    case bound:
      if (not isboundary(*i)) {
        begin = i;
        state = rune;
      }
      break;
    }
  }
  if (state == rune) {
    DbSlice slice(*this, begin, end);
    f(slice);
  }
}

////////////////////////////////////////////////////////////////////////////////
DbLine::iterator DbLine::find(int pos) {
  auto end = line.end();
  for (auto i = line.begin(); i != end; i++) {
    if (i->id == pos) {
      return i;
    }
  }
  return end;
}

////////////////////////////////////////////////////////////////////////////////
DbLine::iterator DbLine::next(iterator b, int len) {
  const auto end = line.end();
  for (auto i = b; i != end; i++) {
    if (len < 0 and std::iswspace(i->get_cor())) {
      return i;
    }
    if (len == 0) {
      return i;
    }
    --len;
  }
  return end;
}

////////////////////////////////////////////////////////////////////////////////
DbSlice DbLine::slice() {
  if (line.empty()) {
    return DbSlice(*this, line.end(), line.end());
  }
  return DbSlice(*this, line.begin(), line.end());
}

////////////////////////////////////////////////////////////////////////////////
DbSlice DbLine::slice(int begin, int len) {
  auto b = find(begin);
  return DbSlice(*this, b, next(b, len));
}

////////////////////////////////////////////////////////////////////////////////
void DbLine::updateOCR(const std::wstring &ocr, const std::vector<int> &cuts,
                       const std::vector<double> &confs) {
  line.clear();
  for (size_t i = 0; i < ocr.size(); i++) {
    assert(i < cuts.size());
    DbChar c{ocr[i], 0, 0.0, cuts[i], int(i), false};
    if (i < confs.size()) {
      c.conf = confs[i];
    }
    line.push_back(c);
  }
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
bool Statistics::load(MysqlConnection &mysql, const DbPackage &pkg) {
  bookid = pkg.bookid;
  projectid = pkg.projectid;
  for (const auto pid : pkg.pageids) {
    DbPage page(pkg.bookid, pid);
    if (not page.load(mysql)) {
      return false;
    }
    for (auto &line : page.lines) {
      lines++;
      if (line.slice().is_manually_corrected()) {
        corLines++;
      }
      line.each_token([&](auto &token) {
        tokens++;
        // if (token.is_automatically_corrected()) {
        //   autoTokens++;
        // }
        if (token.is_manually_corrected()) {
          corTokens++;
          // corrected tokens whose ocr was correct
          if (token.wocr() == token.wcor()) {
            ocrCorTokens++;
          }
        }
      });
    }
  }
  // calculate autocorrections
  using namespace sqlpp;
  tables::Autocorrections acs;
  const auto all = mysql.db()(
      select(count(acs.ocrtypid)).from(acs).where(acs.bookid == pkg.bookid));
  if (not all.empty()) {
    acTokens = all.front().count;
  }
  const auto cors =
      mysql.db()(select(count(acs.ocrtypid))
                     .from(acs)
                     .where(acs.bookid == pkg.bookid and acs.taken == true));
  if (not cors.empty()) {
    acCorTokens = cors.front().count;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, const DbSlice &slice) {
  j["bookId"] = slice.bookid;
  j["projectId"] = slice.projectid;
  j["pageId"] = slice.pageid;
  j["lineId"] = slice.lineid;
  j["tokenId"] = slice.tokenid();
  j["offset"] = slice.offset();
  j["box"] << slice.box;
  j["cor"] = slice.cor();
  j["ocr"] = slice.ocr();
  j["cuts"] = slice.cuts();
  j["confidences"] = slice.confs();
  j["averageConfidence"] = fix_double(slice.average_conf());
  j["isAutomaticallyCorrected"] = slice.is_automatically_corrected();
  j["isManuallyCorrected"] = slice.is_manually_corrected();
  j["match"] = slice.match;
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, DbLine &line) {
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
  j["isAutomaticallyCorrected"] = slice.is_automatically_corrected();
  j["isManuallyCorrected"] = slice.is_manually_corrected();
  j["tokens"] = crow::json::rvalue(crow::json::type::List);
  auto i = 0;
  line.each_token([&](const auto &token) { j["tokens"][i++] << token; });
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &j, DbPage &page) {
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
  for (auto &line : page.lines) {
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
  j["pooled"] = package.pooled;
  j["status"]["profiled"] = package.profiled;
  j["status"]["post-corrected"] = package.postCorrected;
  j["status"]["extended-lexicon"] = package.extendedLexicon;
  return j;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(pcw::Json &j, const Statistics &s) {
  j["lines"] = s.lines;
  j["corLines"] = s.corLines;
  j["tokens"] = s.tokens;
  j["corTokens"] = s.corTokens;
  j["ocrCorTokens"] = s.ocrCorTokens;
  j["acTokens"] = s.acTokens;
  j["acCorTokens"] = s.acCorTokens;
  return j;
}
