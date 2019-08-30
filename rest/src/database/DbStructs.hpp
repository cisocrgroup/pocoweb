#ifndef pcw_DbStructs_hpp__
#define pcw_DbStructs_hpp__

#include "Tables.h"
#include "core/Box.hpp"
#include "mysql.hpp"
#include <boost/optional.hpp>
#include <functional>
#include <sqlpp11/sqlpp11.h>

namespace crow {
namespace json {
class wvalue;
} // namespace json
} // namespace crow

namespace pcw {
using Json = crow::json::wvalue;

struct DbChar {
  static const wchar_t DEL = -1;
  bool is_del() const noexcept { return cor == DEL; }
  bool is_ins() const noexcept { return cor and not ocr; }
  bool is_subst() const noexcept { return cor and ocr and cor != ocr; }
  bool is_cor() const noexcept { return cor; }
  wchar_t get_cor() const noexcept {
    return is_del() ? 0 : is_cor() ? cor : ocr;
  }

  wchar_t ocr, cor;
  double conf;
  int cut;
};

struct DbSlice {
  std::string ocr() const;
  std::string cor() const;
  std::wstring wocr() const;
  std::wstring wcor() const;
  std::vector<int> cuts() const;
  std::vector<double> confs() const;
  double average_conf() const;
  bool is_partially_corrected() const;
  bool is_fully_corrected() const;

  int bookid, projectid, pageid, lineid, offset;
  std::vector<DbChar>::const_iterator begin, end;
  Box box;
};

Json &operator<<(Json &j, const DbSlice &line);

struct DbLine {
  DbLine(int pid, int pageid, int lid)
      : line(), imagepath(), box(), bookid(), projectid(pid), pageid(pageid),
        lineid(lid), offset_() {}
  bool load(MysqlConnection &mysql);

  // slices
  void each_token(std::function<void(const DbSlice &)> f) const;
  DbSlice slice() const { return slice(0, line.size()); }
  DbSlice slice(int begin, int len) const;
  int tokenLength(int begin) const;

  // wagner-fischer interface
  void begin_wagner_fischer(size_t b, size_t e);
  void insert(size_t i, wchar_t c);
  void erase(size_t i);
  void set(size_t i, wchar_t c);
  void noop(size_t i);
  void end_wagner_fischer() const noexcept {}

  std::vector<DbChar> line;
  std::string imagepath;
  Box box;
  int bookid, projectid, pageid, lineid;

private:
  int offset_;
};

Json &operator<<(Json &j, const DbLine &line);

struct DbPage {
  DbPage(int pid, int pageid)
      : box(), lines(), ocrpath(), imagepath(), bookid(), projectid(pid),
        pageid(pageid), filetype(), prevpageid(pageid), nextpageid(pageid) {}
  bool load(MysqlConnection &mysql);

  Box box;
  std::vector<DbLine> lines;
  std::string ocrpath, imagepath;
  int bookid, projectid, pageid, filetype, prevpageid, nextpageid;
};

Json &operator<<(Json &j, const DbPage &page);

struct DbPackage {
  DbPackage(int pid)
      : pageids(), title(), author(), description(), uri(), profilerurl(),
        histpatterns(), directory(), language(), bookid(pid), projectid(pid),
        owner(), year(), profiled(), extendedLexicon(), postCorrected() {}
  bool load(MysqlConnection &mysql);

  std::vector<int> pageids;
  std::string title, author, description, uri, profilerurl, histpatterns,
      directory, language;
  int bookid, projectid, owner, year;
  bool profiled, extendedLexicon, postCorrected;
};

Json &operator<<(Json &j, const DbPackage &package);

////////////////////////////////////////////////////////////////////////////////
inline auto project_line_contents_view() {
  using namespace sqlpp;
  tables::Contents c;
  tables::Textlines l;
  tables::Projects p;
  tables::ProjectPages pp;
  return select(p.id, all_of(l), c.cut, c.conf, c.ocr, c.cor)
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
}

} // namespace pcw
#endif // pcw_DbStructs_hpp__
