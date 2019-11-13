#ifndef pcw_DbStructs_hpp__
#define pcw_DbStructs_hpp__

#include "Tables.h"
#include "common.hpp"
#include "core/Box.hpp"
#include "core/rapidjson.hpp"
#include "mysql.hpp"
#include <boost/optional.hpp>
#include <functional>
#include <list>
#include <sqlpp11/sqlpp11.h>
#include <vector>
namespace boost {
namespace filesystem {
class path;
}
} // namespace boost
namespace crow {
namespace json {
class wvalue;
class rvalue;
} // namespace json
} // namespace crow
namespace pcw {
class WagnerFischer;
struct DbLine;
using Json = crow::json::wvalue;
using RJson = crow::json::rvalue;
using Path = boost::filesystem::path;

// DbChar represents an ocr char in a line.  The member cut
// represents the offset of the right side of the char and conf give
// the confidence of the ocr char.  Ocr defines the original
// recognized ocr character and cor its correction.  If ocr=0, we
// have an insertion, if cor=-1 we have a deletion and if cor=0, the
// char was not yet corrected.
struct DbChar {
  static const wchar_t DEL = -1;
  bool is_del() const noexcept { return cor == DEL; }
  bool is_ins() const noexcept { return ocr == 0 and cor != DEL; }
  bool is_subst() const noexcept {
    return cor != 0 and ocr != 0 and cor != ocr;
  }
  bool is_cor() const noexcept { return cor != 0; }
  wchar_t get_cor() const noexcept {
    return is_del() ? 0 : is_cor() ? cor : ocr;
  }
  int id(int offset) { return is_ins() ? offset + MAX_LINE_LENGTH : offset; }

  wchar_t ocr, cor;
  double conf;
  int cut;
  bool manually;
};

struct DbSlice {
  using iterator = std::list<DbChar>::iterator;
  DbSlice(DbLine &line, iterator b, iterator e, int tid, int offset);

  std::string ocr() const;
  std::string cor() const;
  std::wstring wocr() const;
  std::wstring wcor() const;
  std::vector<int> cuts() const;
  std::vector<double> confs() const;
  double average_conf() const;
  bool is_automatically_corrected() const;
  bool is_manually_corrected() const;
  bool contains_manual_corrections() const;
  DbLine &line() { return *line_; }

  // wagner-fischer interface
  void begin_wagner_fischer(size_t b, size_t e);
  void insert(size_t i, wchar_t c);
  void erase(size_t i);
  void set(size_t i, wchar_t c);
  void noop(size_t i);
  void end_wagner_fischer() const noexcept {}
  void set_correction_type(bool manual);

  template <class OS> void serialize(rapidjson::Writer<OS> &w) const;
  std::string strID() const {
    return std::to_string(projectid) + ":" + std::to_string(pageid) + ":" +
           std::to_string(lineid) + ":" + std::to_string(offset);
  }

  int bookid, projectid, pageid, lineid, tokenid, offset;
  std::list<DbChar>::iterator begin, end;
  Box box;
  bool match;

private:
  DbLine *line_;
  std::list<DbChar>::iterator i_;
};

template <class OS> void DbSlice::serialize(rapidjson::Writer<OS> &w) const {
  w.StartObject();
  w.String("bookId");
  w.Int(bookid);
  w.String("projectId");
  w.Int(projectid);
  w.String("pageId");
  w.Int(pageid);
  w.String("lineId");
  w.Int(lineid);
  w.String("tokenId");
  w.Int(tokenid);
  w.String("offset");
  w.Int(offset);
  w.String("box");
  box.serialize(w);
  w.String("cor");
  w.String(cor());
  w.String("ocr");
  w.String(ocr());
  w.String("cuts");
  w.StartArray();
  std::for_each(begin, end, [&](const auto &c) { w.Int(c.cut); });
  w.EndArray();
  w.String("confidences");
  w.StartArray();
  std::for_each(begin, end, [&](const auto &c) { w.Double(c.conf); });
  w.EndArray();
  w.String("averageConfidence");
  fixed_double(w, average_conf());
  w.String("isAutomaticallyCorrected");
  w.Bool(is_automatically_corrected());
  w.String("isManuallyCorrected");
  w.Bool(is_manually_corrected());
  w.String("match");
  w.Bool(match);
  w.EndObject();
}

Json &operator<<(Json &j, const DbSlice &line);

struct DbLine {
  using iterator = std::list<DbChar>::iterator;
  using pos = struct {
    iterator it;
    int id, offset;
  };
  DbLine(int pid, int pageid, int lid)
      : line(), imagepath(), box(), bookid(), projectid(pid), pageid(pageid),
        lineid(lid), begin_(), end_() {}
  bool load(MysqlConnection &mysql);

  // slices
  pos find(int pos);
  // len = -1 means until first encountered whitespace.
  pos next(pos begin, int len);
  void each_token(std::function<void(DbSlice &)> f);
  DbSlice slice();
  // len = -1 means slice from begin to next whitespace.
  DbSlice slice(int begin, int len);

  // update
  void updateOCR(const std::wstring &ocr, const std::vector<int> &cuts,
                 const std::vector<double> &confs);
  void updateImage(const std::string &base, const std::string &imagedata) const;
  void updateContents(MysqlConnection &mysql) const;

  template <class It, class F> void iterate(It b, It e, F f);
  template <class OS> void serialize(rapidjson::Writer<OS> &w);
  std::string strID() const {
    return std::to_string(projectid) + ":" + std::to_string(pageid) + ":" +
           std::to_string(lineid);
  }
  std::list<DbChar> line;
  std::string imagepath;
  Box box;
  int bookid, projectid, pageid, lineid;

private:
  void clear_insertions();
  std::list<DbChar>::iterator begin_, end_;
};

template <class It, class F> void DbLine::iterate(It b, It e, F f) {
  int offset = 0;
  int ocrid = 0;
  int insid = MAX_LINE_LENGTH;
  while (b != e) {
    int id;
    if (b->is_ins()) {
      id = insid++;
    } else {
      id = ocrid++;
    }
    b = f(b, id, offset++);
  }
}

template <class OS> void DbLine::serialize(rapidjson::Writer<OS> &w) {
  const auto slice = this->slice();
  w.StartObject();
  w.String("bookId");
  w.Int(bookid);
  w.String("projectId");
  w.Int(projectid);
  w.String("pageId");
  w.Int(pageid);
  w.String("lineId");
  w.Int(lineid);
  w.String("box");
  box.serialize(w);
  w.String("cor");
  w.String(slice.cor());
  w.String("ocr");
  w.String(slice.ocr());
  w.String("imgFile");
  w.String(imagepath);
  w.String("cuts");
  w.StartArray();
  std::for_each(slice.begin, slice.end, [&](const auto &c) { w.Int(c.cut); });
  w.EndArray();
  w.String("confidences");
  w.StartArray();
  std::for_each(slice.begin, slice.end,
                [&](const auto &c) { fixed_double(w, c.conf); });
  w.EndArray();
  w.String("averageConfidence");
  fixed_double(w, slice.average_conf());
  w.String("isAutomaticallyCorrected");
  w.Bool(slice.is_automatically_corrected());
  w.String("isManuallyCorrected");
  w.Bool(slice.is_manually_corrected());
  w.String("tokens");
  w.StartArray();
  each_token([&](const auto &token) { token.serialize(w); });
  w.EndArray();
  w.EndObject();
}

inline bool operator==(const DbLine &lhs, const DbLine &rhs) {
  return lhs.bookid == rhs.bookid and lhs.projectid == lhs.projectid and
         lhs.pageid == rhs.pageid and lhs.lineid == rhs.lineid;
}

inline bool operator!=(const DbLine &lhs, const DbLine &rhs) {
  return not operator==(lhs, rhs);
}

Json &operator<<(Json &j, DbLine &line);

struct DbPage {
  DbPage(int pid, int pageid)
      : box(), lines(), ocrpath(), imagepath(), bookid(), projectid(pid),
        pageid(pageid), filetype(), prevpageid(pageid), nextpageid(pageid) {}
  bool load(MysqlConnection &mysql);
  template <class OS> void serialize(rapidjson::Writer<OS> &w);
  std::string strID() const {
    return std::to_string(projectid) + ":" + std::to_string(pageid);
  }

  Box box;
  std::vector<DbLine> lines;
  std::string ocrpath, imagepath;
  int bookid, projectid, pageid, filetype, prevpageid, nextpageid;
};

template <class OS> void DbPage::serialize(rapidjson::Writer<OS> &w) {
  w.StartObject();
  w.String("bookId");
  w.Int(bookid);
  w.String("projectId");
  w.Int(projectid);
  w.String("pageId");
  w.Int(pageid);
  w.String("prevPageId");
  w.Int(prevpageid);
  w.String("nextPageId");
  w.Int(nextpageid);
  w.String("imgFile");
  w.String(imagepath);
  w.String("ocrFile");
  w.String(ocrpath);
  w.String("box");
  box.serialize(w);
  w.String("lines");
  w.StartArray();
  std::for_each(lines.begin(), lines.end(),
                [&](auto &line) { line.serialize(w); });
  w.EndArray();
  w.EndObject();
}

Json &operator<<(Json &j, DbPage &page);
struct DbPackage;

struct Statistics {
  bool load(MysqlConnection &mysq, const DbPackage &pkg);
  template <class OS> void serialize(rapidjson::Writer<OS> &w) const;
  size_t lines, corLines, tokens, corTokens, ocrCorTokens, acTokens,
      acCorTokens;
  int bookid, projectid;
};

template <class OS> void Statistics::serialize(rapidjson::Writer<OS> &w) const {
  w.StartObject();
  w.String("bookId");
  w.Int(bookid);
  w.String("projectId");
  w.Int(projectid);
  w.String("lines");
  w.Uint(lines);
  w.String("corLines");
  w.Uint(corLines);
  w.String("tokens");
  w.Uint(tokens);
  w.String("corTokens");
  w.Uint(corTokens);
  w.String("ocrCorTokens");
  w.Uint(ocrCorTokens);
  w.String("acTokens");
  w.Uint(acTokens);
  w.String("acCorTokens");
  w.Uint(acCorTokens);
  w.EndObject();
}

Json &operator<<(Json &j, const Statistics &stats);

struct DbPackage {
  DbPackage(int pid)
      : pageids(), title(), author(), description(), uri(), profilerurl(),
        histpatterns(), directory(), language(), bookid(pid), projectid(pid),
        owner(), year(), profiled(), extendedLexicon(), postCorrected(),
        pooled() {}
  bool load(MysqlConnection &mysql);
  bool isBook() const noexcept { return projectid == bookid; }
  template <class OS> void serialize(rapidjson::Writer<OS> &w) const;
  std::string strID() const { return std::to_string(projectid); }
  std::vector<int> pageids;
  std::string title, author, description, uri, profilerurl, histpatterns,
      directory, language;
  int bookid, projectid, owner, year;
  bool profiled, extendedLexicon, postCorrected, pooled;
};

template <class OS> void DbPackage::serialize(rapidjson::Writer<OS> &w) const {
  w.StartObject();
  w.String("bookId");
  w.Int(bookid);
  w.String("projectId");
  w.Int(projectid);
  w.String("owner");
  w.Int(owner);
  w.String("title");
  w.String(title);
  w.String("author");
  w.String(author);
  w.String("description");
  w.String(description);
  w.String("profilerUrl");
  w.String(profilerurl);
  w.String("histpatterns");
  w.String(histpatterns);
  w.String("uri");
  w.String(uri);
  w.String("directory");
  w.String(directory);
  w.String("language");
  w.String(language);
  w.String("year");
  w.Int(year);
  w.String("isBook");
  w.Bool(bookid == projectid);
  w.String("pooled");
  w.Bool(pooled);
  w.String("status");
  w.StartObject();
  w.String("profiled");
  w.Bool(profiled);
  w.String("post-corrected");
  w.Bool(postCorrected);
  w.String("extended-lexicon");
  w.Bool(extendedLexicon);
  w.EndObject();
  w.String("pages");
  w.Int(int(pageids.size()));
  w.String("pageIds");
  w.StartArray();
  std::for_each(pageids.begin(), pageids.end(), [&](int id) { w.Int(id); });
  w.EndArray();
  w.EndObject();
}

Json &operator<<(Json &j, const DbPackage &package);

////////////////////////////////////////////////////////////////////////////////
inline auto project_line_contents_view() {
  using namespace sqlpp;
  tables::Contents c;
  tables::Textlines l;
  tables::Projects p;
  tables::ProjectPages pp;
  return select(p.id, all_of(l), c.cut, c.conf, c.ocr, c.cor, c.manually)
      .from(p.join(l)
                .on(p.origin == l.bookid)
                .join(pp)
                .on(pp.pageid == l.pageid and pp.projectid == p.id)
                .join(c)
                .on(c.bookid == l.bookid and c.pageid == l.pageid and
                    c.lineid == l.lineid));
}

////////////////////////////////////////////////////////////////////////////////
inline auto project_pages_view() {
  using namespace sqlpp;
  tables::ProjectPages pp;
  tables::Pages pages;
  tables::Projects p;
  return select(p.id, all_of(pages), pp.nextpageid, pp.prevpageid)
      .from(p.join(pages)
                .on(p.origin == pages.bookid)
                .join(pp)
                .on(pp.pageid == pages.pageid and pp.projectid == p.id));
}

////////////////////////////////////////////////////////////////////////////////
inline auto project_book_pages_view() {
  using namespace sqlpp;
  tables::Projects p;
  tables::Books b;
  tables::ProjectPages pp;
  return select(all_of(b), p.id, p.owner, pp.pageid)
      .from(
          b.join(p).on(p.origin == b.bookid).join(pp).on(p.id == pp.projectid));
}

////////////////////////////////////////////////////////////////////////////////
inline auto project_books_view() {
  using namespace sqlpp;
  tables::Projects p;
  tables::Books b;
  return select(all_of(b), p.id, p.owner)
      .from(b.join(p).on(p.origin == b.bookid));
}

////////////////////////////////////////////////////////////////////////////////
template <class Row> void load_from_row(Row &row, DbPage &page) {
  page.box =
      Box{int(row.pleft), int(row.ptop), int(row.pright), int(row.pbottom)};
  page.imagepath = row.imagepath;
  page.ocrpath = row.ocrpath;
  page.bookid = row.bookid;
  page.projectid = row.id;
  page.filetype = row.filetype;
  page.pageid = row.pageid;
  page.prevpageid = row.prevpageid;
  page.nextpageid = row.nextpageid;
}

////////////////////////////////////////////////////////////////////////////////
template <class Row> void load_from_row(Row &row, DbPackage &package) {
  package.bookid = row.bookid;
  package.projectid = row.id;
  package.author = row.author;
  package.description = row.description;
  package.title = row.title;
  package.profilerurl = row.profilerurl;
  package.uri = row.uri;
  package.directory = row.directory;
  package.language = row.lang;
  package.histpatterns = row.histpatterns;
  package.profiled = row.profiled;
  package.extendedLexicon = row.extendedlexicon;
  package.postCorrected = row.postcorrected;
  package.owner = row.owner;
  package.year = row.year;
  package.pooled = row.pooled;
}

////////////////////////////////////////////////////////////////////////////////
template <class Row> void load_from_row(Row &row, DbLine &line) {
  line.box =
      Box{int(row.lleft), int(row.ltop), int(row.lright), int(row.lbottom)};
  line.imagepath = row.imagepath;
  line.projectid = row.id;
  line.bookid = row.bookid;
  line.pageid = row.pageid;
  line.lineid = row.lineid;
}

////////////////////////////////////////////////////////////////////////////////
template <class Row> void load_from_row(Row &row, DbChar &c) {
  c.ocr = wchar_t(row.ocr);
  c.cor = wchar_t(row.cor);
  c.cut = int(row.cut);
  c.conf = row.conf;
  c.manually = row.manually;
}

} // namespace pcw
#endif // pcw_DbStructs_hpp__
