#ifndef pcw_Database_hpp__
#define pcw_Database_hpp__

#include <crow/logging.h>
#include <sqlpp11/sqlpp11.h>
#include <boost/optional.hpp>
#include <memory>
#include "Tables.h"
#include "core/Book.hpp"
#include "core/BookBuilder.hpp"
#include "core/Line.hpp"
#include "core/LineBuilder.hpp"
#include "core/Page.hpp"
#include "core/PageBuilder.hpp"
#include "core/Password.hpp"
#include "core/Project.hpp"
#include "core/ProjectBuilder.hpp"
#include "core/User.hpp"

namespace pcw {
struct ProjectEntry;

namespace detail {
template <class Row>
FileType to_file_type(const Row& row) {
	const auto ft = static_cast<int>(row.filetype);
	if (ft < static_cast<int>(FileType::Min) or
	    ft > static_cast<int>(FileType::Max))
		throw std::logic_error("invalid file type");
	return static_cast<FileType>(ft);
}

template <class U>
UserSptr make_user(const U& users) {
	return std::make_shared<User>(users.name, Password(users.passwd),
				      users.email, users.institute,
				      users.userid, users.admin);
}
template <class T, class F>
void set_if_not_null(const T& t, F f) {
	if (not t.is_null()) f(t);
}
template <class Row>
BookData make_book_data(const Row& row) noexcept;

template <class Row>
ProjectEntry make_project_entry(const Row& row) noexcept;

template <class Db, class P, class Q, class R>
void insert_page(Db& db, P& p, Q& q, R& r, const Page& page);

template <class Db, class Q, class R>
void insert_line(Db& db, Q& q, R& r, const Line& line);

template <class Db, class R>
void insert_content(Db& db, R& r, const Line& line);

template <class Db>
void insert_book_info_and_set_id(Db& db, Book& book);

template <class Db>
void insert_project_info_and_set_id(Db& db, Project& view);

template <class Db>
void delete_project(Db& db, int pid);

template <class Db>
void select_pages(Db& db, const BookBuilder& builder, int bookid);

template <class Db>
void select_lines(Db& db, const PageBuilder& builder, int bookid, int pageid);

template <class Db>
void select_contents(Db& db, const LineBuilder& builder, int bookid, int pageid,
		     int lineid);
}

template <class Db>
UserSptr create_user(Db& db, const std::string& user, const std::string& pw,
		     const std::string& email = "",
		     const std::string& inst = "", bool admin = false);

template <class Db>
void update_user(Db& db, const User& user);

template <class Db>
UserSptr select_user(Db& db, const std::string& name);

template <class Db>
UserSptr select_user(Db& db, int id);

template <class Db>
std::vector<UserSptr> select_all_users(Db& db);

template <class Db>
void delete_user(Db& db, const std::string& name);

template <class Db>
void delete_user(Db& db, int id);

template <class Db>
ProjectSptr insert_project(Db& db, Project& book);

template <class Db>
BookSptr insert_book(Db& db, Book& book);

template <class Db>
void update_book(Db& db, Project& view);

struct ProjectEntry {
	bool is_book() const noexcept { return origin == projectid; }
	int origin, owner, projectid, pages;
};

template <class Db>
boost::optional<ProjectEntry> select_project_entry(Db& db, int projectid);

template <class Db>
std::vector<ProjectEntry> select_all_project_entries(Db& db, const User& owner);

template <class Db>
boost::optional<BookData> select_book_data(Db& db, int bookid);

template <class Db>
std::vector<std::pair<BookData, ProjectEntry>> select_all_projects(
    Db& db, const User& user);

template <class Db>
void update_project_owner(Db& db, int projectid, int owner);

template <class Db>
BookSptr select_book(Db& db, const User& owner, int bookid);

template <class Db>
ProjectSptr select_project(Db& db, const Book& book, int projectid);

template <class Db>
void update_line(Db& db, const Line& line);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::UserSptr pcw::create_user(Db& db, const std::string& name,
			       const std::string& pw, const std::string& email,
			       const std::string& inst, bool admin) {
	using namespace sqlpp;
	auto password = Password::make(pw);
	tables::Users users;
	auto stmnt = insert_into(users).set(
	    users.name = name, users.email = email, users.institute = inst,
	    users.passwd = password.str(), users.admin = admin);
	auto id = db(stmnt);
	return std::make_shared<User>(name, password, email, inst, id);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::update_user(Db& db, const User& user) {
	using namespace sqlpp;
	tables::Users users;
	// do not change user's id
	auto stmnt =
	    update(users)
		.set(users.passwd = user.password.str(), users.name = user.name,
		     users.email = user.email, users.institute = user.institute)
		.where(users.userid == user.id());
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::UserSptr pcw::select_user(Db& db, const std::string& name) {
	using namespace sqlpp;
	tables::Users users;
	auto stmnt =
	    select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmnt);
	if (not res.empty()) return pcw::detail::make_user(res.front());
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::UserSptr pcw::select_user(Db& db, int id) {
	using namespace sqlpp;
	tables::Users users;
	auto stmnt =
	    select(all_of(users)).from(users).where(users.userid == id);
	auto res = db(stmnt);
	if (not res.empty()) return pcw::detail::make_user(res.front());
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
std::vector<pcw::UserSptr> pcw::select_all_users(Db& db) {
	using namespace sqlpp;
	std::vector<UserSptr> res;
	tables::Users users;
	const auto stmnt = select(all_of(users)).from(users).unconditionally();
	// const auto rows = db(stmnt);
	// while (not rows.empty()) {
	for (const auto& row : db(stmnt)) {
		res.push_back(detail::make_user(row));
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::delete_user(Db& db, const std::string& name) {
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = remove_from(users).where(users.name == name);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::delete_user(Db& db, int id) {
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = remove_from(users).where(users.userid == id);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::detail::insert_project_info_and_set_id(Db& db, Project& view) {
	using namespace sqlpp;
	tables::Projects projects;
	auto id = db(insert_into(projects).set(
	    projects.origin = view.origin().id(),
	    projects.pages = static_cast<int>(view.size()),
	    projects.owner = view.owner().id()));
	view.set_id(id);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::ProjectSptr pcw::insert_project(Db& db, Project& view) {
	using namespace sqlpp;
	detail::insert_project_info_and_set_id(db, view);

	tables::ProjectPages project_pages;
	auto stmnt = db.prepare(
	    insert_into(project_pages)
		.set(project_pages.projectid = view.id(),
		     project_pages.pageid = parameter(project_pages.pageid)));
	for (const auto& page : view) {
		assert(page);
		stmnt.params.pageid = page->id();
		db(stmnt);
	}
	return view.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::detail::insert_book_info_and_set_id(Db& db, Book& book) {
	using namespace sqlpp;
	tables::Projects projects;
	auto id = db(insert_into(projects).set(
	    projects.origin = 0, projects.owner = book.owner().id(),
	    projects.pages = book.size()));
	CROW_LOG_DEBUG << "(insert_book_info_and_set_id) id: " << id;
	book.set_id(id);
	db(update(projects)
	       .set(projects.origin = id)
	       .where(projects.projectid == id));
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::BookSptr pcw::insert_book(Db& db, Book& book) {
	using namespace sqlpp;
	detail::insert_book_info_and_set_id(db, book);

	tables::Books books;
	auto stmnt = insert_into(books).set(
	    books.author = book.data.author, books.title = book.data.title,
	    books.directory = book.data.dir.string(),
	    books.year = book.data.year, books.uri = book.data.uri,
	    books.bookid = book.id(), books.description = book.data.description,
	    books.lang = book.data.lang);
	CROW_LOG_DEBUG << "(insert_book) bookid:      " << book.id();
	CROW_LOG_DEBUG << "(insert_book) author:      " << book.data.author;
	CROW_LOG_DEBUG << "(insert_book) title:       " << book.data.title;
	CROW_LOG_DEBUG << "(insert_book) directory:   " << book.data.dir;
	CROW_LOG_DEBUG << "(insert_book) year:        " << book.data.year;
	CROW_LOG_DEBUG << "(insert_book) uri:         " << book.data.author;
	CROW_LOG_DEBUG << "(insert_book) description: "
		       << book.data.description;
	CROW_LOG_DEBUG << "(insert_book) language:    " << book.data.lang;
	db(stmnt);

	tables::Pages pages;
	auto p = db.prepare(
	    insert_into(pages).set(pages.bookid = parameter(pages.bookid),
				   pages.pageid = parameter(pages.pageid),
				   pages.imagepath = parameter(pages.imagepath),
				   pages.ocrpath = parameter(pages.ocrpath),
				   pages.filetype = parameter(pages.filetype),
				   pages.pleft = parameter(pages.pleft),
				   pages.ptop = parameter(pages.ptop),
				   pages.pright = parameter(pages.pright),
				   pages.pbottom = parameter(pages.pbottom)));

	tables::Textlines textlines;
	auto q = db.prepare(insert_into(textlines).set(
	    textlines.bookid = parameter(textlines.bookid),
	    textlines.pageid = parameter(textlines.pageid),
	    textlines.lineid = parameter(textlines.lineid),
	    textlines.imagepath = parameter(textlines.imagepath),
	    textlines.lleft = parameter(textlines.lleft),
	    textlines.ltop = parameter(textlines.ltop),
	    textlines.lright = parameter(textlines.lright),
	    textlines.lbottom = parameter(textlines.lbottom)));
	tables::Contents contents;
	auto r = db.prepare(insert_into(contents).set(
	    contents.bookid = parameter(contents.bookid),
	    contents.pageid = parameter(contents.pageid),
	    contents.lineid = parameter(contents.lineid),
	    contents.seq = parameter(contents.seq),
	    contents.ocr = parameter(contents.ocr),
	    contents.cor = parameter(contents.cor),
	    contents.cut = parameter(contents.cut),
	    contents.conf = parameter(contents.conf)));
	for (const auto& page : book) {
		assert(page);
		detail::insert_page(db, p, q, r, *page);
	}
	return std::static_pointer_cast<Book>(book.shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
template <class Db, class P, class Q, class R>
void pcw::detail::insert_page(Db& db, P& p, Q& q, R& r, const Page& page) {
	p.params.bookid = page.book().id();
	p.params.pageid = page.id();
	p.params.imagepath = page.img.string();
	p.params.ocrpath = page.ocr.string();
	p.params.filetype = static_cast<int>(page.file_type);
	p.params.pleft = page.box.left();
	p.params.ptop = page.box.top();
	p.params.pright = page.box.right();
	p.params.pbottom = page.box.bottom();
	CROW_LOG_DEBUG << "(insert_page) bookid:    " << page.book().id();
	CROW_LOG_DEBUG << "(insert_page) pageid:    " << page.id();
	CROW_LOG_DEBUG << "(insert_page) imagepath: " << page.img.string();
	CROW_LOG_DEBUG << "(insert_page) ocrpath:   " << page.ocr.string();
	// CROW_LOG_DEBUG << "(insert_page) pleft:     " << page.box.left();
	// CROW_LOG_DEBUG << "(insert_page) ptop:      " << page.box.top();
	// CROW_LOG_DEBUG << "(insert_page) pright:    " << page.box.right();
	// CROW_LOG_DEBUG << "(insert_page) pbottom:   " << page.box.bottom();
	db(p);
	for (const auto& line : page) {
		assert(line);
		detail::insert_line(db, q, r, *line);
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class Db, class Q, class R>
void pcw::detail::insert_line(Db& db, Q& q, R& r, const Line& line) {
	q.params.bookid = line.page().book().id();
	q.params.pageid = line.page().id();
	q.params.lineid = line.id();
	q.params.imagepath = line.img.string();
	q.params.lleft = line.box.left();
	q.params.ltop = line.box.top();
	q.params.lright = line.box.right();
	q.params.lbottom = line.box.bottom();
	CROW_LOG_DEBUG << "(insert_line) bookid:    "
		       << line.page().book().id();
	CROW_LOG_DEBUG << "(insert_line) pageid:    " << line.page().id();
	CROW_LOG_DEBUG << "(insert_line) lineid:    " << line.id();
	CROW_LOG_DEBUG << "(insert_line) imagepath: " << line.img.string();
	// CROW_LOG_DEBUG << "(insert_line) lleft:     " << line.box.left();
	// CROW_LOG_DEBUG << "(insert_line) ltop:      " << line.box.top();
	// CROW_LOG_DEBUG << "(insert_line) lright:    " << line.box.right();
	// CROW_LOG_DEBUG << "(insert_line) lbottom:   " << line.box.bottom();
	db(q);
	insert_content(db, r, line);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db, class R>
void pcw::detail::insert_content(Db& db, R& r, const Line& line) {
	r.params.bookid = line.page().book().id();
	r.params.pageid = line.page().id();
	r.params.lineid = line.id();
	for (auto i = 0U; i < line.size(); ++i) {
		r.params.seq = i;
		r.params.ocr = line[i].ocr;
		r.params.cor = line[i].cor;
		r.params.cut = line[i].cut;
		r.params.conf = line[i].conf;
		db(r);
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::update_book(Db& db, Project& view) {
	using namespace sqlpp;
	tables::Books books;
	auto stmnt =
	    update(books)
		.set(books.author = view.origin().data.author,
		     books.title = view.origin().data.title,
		     books.directory = view.origin().data.dir.string(),
		     books.year = view.origin().data.year,
		     books.uri = view.origin().data.uri,
		     books.description = view.origin().data.description,
		     books.lang = view.origin().data.lang)
		.where(books.bookid == view.origin().id());
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
boost::optional<pcw::ProjectEntry> pcw::select_project_entry(Db& db,
							     int projectid) {
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = select(all_of(projects))
			 .from(projects)
			 .where(projects.projectid == projectid);
	auto res = db(stmnt);
	if (not res.empty()) return detail::make_project_entry(res.front());
	return {};
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
std::vector<std::pair<pcw::BookData, pcw::ProjectEntry>>
pcw::select_all_projects(Db& db, const User& user) {
	using namespace sqlpp;
	tables::Projects projects;
	tables::Books books;
	auto stmnt =
	    select(all_of(projects), all_of(books))
		.from(books.join(projects).on(books.bookid == projects.origin))
		.where(projects.owner == user.id());
	std::vector<std::pair<BookData, ProjectEntry>> data;
	for (const auto& row : db(stmnt)) {
		const auto bookdata = detail::make_book_data(row);
		const auto projectentry = detail::make_project_entry(row);
		data.emplace_back(bookdata, projectentry);
	}
	return data;
}

////////////////////////////////////////////////////////////////////////////////
template <class Row>
pcw::ProjectEntry pcw::detail::make_project_entry(const Row& row) noexcept {
	ProjectEntry entry;
	set_if_not_null(row.projectid, [&](const auto pid) {
		return entry.projectid = static_cast<int>(pid);
	});
	set_if_not_null(row.owner, [&](const auto owner) {
		return entry.owner = static_cast<int>(owner);
	});
	set_if_not_null(row.origin, [&](const auto origin) {
		return entry.origin = static_cast<int>(origin);
	});
	set_if_not_null(row.pages, [&](const auto pages) {
		return entry.pages = static_cast<int>(pages);
	});
	return entry;
}

////////////////////////////////////////////////////////////////////////////////
template <class Row>
pcw::BookData pcw::detail::make_book_data(const Row& row) noexcept {
	BookData data;
	detail::set_if_not_null(row.description,
				[&](const auto& d) { data.description = d; });
	detail::set_if_not_null(row.uri, [&](const auto& u) { data.uri = u; });
	detail::set_if_not_null(row.year,
				[&](const auto& y) { data.year = y; });
	detail::set_if_not_null(row.author,
				[&](const auto& a) { data.author = a; });
	detail::set_if_not_null(row.title,
				[&](const auto& t) { data.title = t; });
	detail::set_if_not_null(row.lang,
				[&](const auto& l) { data.lang = l; });
	detail::set_if_not_null(row.directory,
				[&](const auto& d) { data.dir = d; });
	detail::set_if_not_null(row.profilerurl,
				[&](const auto& u) { data.profilerUrl = u; });
	return data;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
boost::optional<pcw::BookData> pcw::select_book_data(Db& db, int bookid) {
	tables::Projects projects;
	tables::Books books;
	auto stmnt =
	    select(all_of(books))
		.from(books.join(projects).on(books.bookid == projects.origin))
		.where(books.bookid == bookid);

	auto res = db(stmnt);
	if (not res.empty()) return detail::make_book_data(res.front());
	return {};
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::update_project_owner(Db& db, int projectid, int owner) {
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = update(projects)
			 .set(projects.owner = owner)
			 .where(projects.projectid == projectid);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::BookSptr pcw::select_book(Db& db, const User& owner, int bookid) {
	auto book_data = select_book_data(db, bookid);
	if (not book_data) return nullptr;
	BookBuilder builder;
	builder.set_id(bookid);
	builder.set_book_data(*book_data);
	builder.set_owner(owner);
	detail::select_pages(db, builder, bookid);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::detail::select_pages(Db& db, const BookBuilder& builder, int bookid) {
	using namespace sqlpp;
	tables::Pages pages;
	auto stmnt =
	    select(all_of(pages)).from(pages).where(pages.bookid == bookid);
	PageBuilder pbuilder;
	for (const auto& row : db(stmnt)) {
		pbuilder.reset();
		detail::set_if_not_null(row.imagepath, [&](const auto& p) {
			pbuilder.set_image_path(Path(p));
		});
		detail::set_if_not_null(row.ocrpath, [&](const auto& p) {
			pbuilder.set_ocr_path(Path(p));
		});
		pbuilder.set_box({static_cast<int>(row.pleft),
				  static_cast<int>(row.ptop),
				  static_cast<int>(row.pright),
				  static_cast<int>(row.pbottom)});
		pbuilder.set_id(row.pageid);
		pbuilder.set_file_type(detail::to_file_type(row));
		detail::select_lines(db, pbuilder, bookid, row.pageid);
		builder.append(*pbuilder.build());
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::detail::select_lines(Db& db, const PageBuilder& builder, int bookid,
			       int pageid) {
	using namespace sqlpp;
	tables::Textlines lines;
	auto stmnt =
	    select(all_of(lines))
		.from(lines)
		.where(lines.bookid == bookid and lines.pageid == pageid)
		.order_by(lines.lineid.asc());
	LineBuilder lbuilder;
	for (const auto& row : db(stmnt)) {
		lbuilder.reset();
		detail::set_if_not_null(row.imagepath, [&](const auto& p) {
			lbuilder.set_image_path(Path(p));
		});
		lbuilder.set_box({static_cast<int>(row.lleft),
				  static_cast<int>(row.ltop),
				  static_cast<int>(row.lright),
				  static_cast<int>(row.lbottom)});
		lbuilder.set_id(row.lineid);
		detail::select_contents(db, lbuilder, bookid, pageid,
					row.lineid);
		builder.append(*lbuilder.build());
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::detail::select_contents(Db& db, const LineBuilder& builder,
				  int bookid, int pageid, int lineid) {
	using namespace sqlpp;
	tables::Contents contents;
	auto stmnt =
	    select(all_of(contents))
		.from(contents)
		.where(contents.bookid == bookid and
		       contents.pageid == pageid and contents.lineid == lineid)
		.order_by(contents.seq.asc());
	for (const auto& row : db(stmnt)) {
		builder.append(static_cast<wchar_t>(row.ocr),
			       static_cast<wchar_t>(row.cor), row.cut,
			       row.conf);
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
pcw::ProjectSptr pcw::select_project(Db& db, const Book& book, int projectid) {
	using namespace sqlpp;
	tables::ProjectPages project_pages;
	auto stmnt = select(project_pages.pageid)
			 .from(project_pages)
			 .where(project_pages.projectid == projectid);
	ProjectBuilder builder;
	builder.set_origin(book);
	builder.set_project_id(projectid);
	for (const auto& row : db(stmnt)) {
		CROW_LOG_DEBUG
		    << "(select_project) adding page id: " << row.pageid
		    << " to project id: " << projectid;
		builder.add_page(row.pageid);
	}
	auto p = builder.build();
	CROW_LOG_DEBUG << "(select_project) project id: " << p->id()
		       << " origin: " << p->origin().id()
		       << " size: " << p->size();
	return p;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void pcw::update_line(Db& db, const Line& line) {
	using namespace sqlpp;
	tables::Contents contents;
	auto remove = remove_from(contents).where(
	    contents.bookid == line.page().book().id() and
	    contents.pageid == line.page().id() and
	    contents.lineid == line.id());
	auto insert = db.prepare(insert_into(contents).set(
	    contents.bookid = parameter(contents.bookid),
	    contents.pageid = parameter(contents.pageid),
	    contents.lineid = parameter(contents.lineid),
	    contents.seq = parameter(contents.seq),
	    contents.ocr = parameter(contents.ocr),
	    contents.cor = parameter(contents.cor),
	    contents.cut = parameter(contents.cut),
	    contents.conf = parameter(contents.conf)));
	db.run(remove);
	detail::insert_content(db, insert, line);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
void delete_project(Db& db, int pid) {
	using namespace sqlpp;
	tables::Books b;
	tables::Projects p;
	tables::ProjectPages pp;
	tables::Profiles ppp;
	tables::Suggestions s;
	tables::Errorpatterns e;
	tables::Adaptivetokens a;
	tables::Textlines l;
	tables::Contents c;
	tables::Types t;
	db(remove_from(b).where(b.bookid == pid));
	db(remove_from(pp).where(pp.projectid == pid));
	db(remove_from(p).where(p.projectid == pid));
	db(remove_from(ppp).where(ppp.bookid == pid));
	db(remove_from(l).where(l.bookid == pid));
	db(remove_from(c).where(c.bookid == pid));
	db(remove_from(s).where(s.bookid == pid));
	db(remove_from(e).where(e.bookid == pid));
	db(remove_from(t).where(t.bookid == pid));
	db(remove_from(a).where(a.bookid == pid));
	db(remove_from(p).where(p.origin == pid));
}

#endif  // pcw_Database_hpp__
