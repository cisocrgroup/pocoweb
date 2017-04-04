#ifndef pcw_NewDatabase_hpp__
#define pcw_NewDatabase_hpp__

#include <boost/optional.hpp>
#include <memory>
#include <sqlpp11/sqlpp11.h>
#include "core/ProjectBuilder.hpp"
#include "core/Password.hpp"
#include "core/BookBuilder.hpp"
#include "core/PageBuilder.hpp"
#include "core/LineBuilder.hpp"
#include "core/Project.hpp"
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
#include "core/User.hpp"
#include "Tables.h"

namespace pcw {
	namespace detail {
		template<class U>
		UserSptr make_user(const U& users) {
			return std::make_shared<User>(
				users.name,
				Password(users.passwd),
				users.email,
				users.institute,
				users.userid
			);
		}
		template<class T, class F>
		void set_if_not_null(const T& t, F f) {
			if (not t.is_null())
				f(t);
		}
		template<class Db, class P, class Q, class R>
		void insert_page(Db& db, P& p, Q& q, R& r, const Page& page);

		template<class Db, class Q, class R>
		void insert_line(Db& db, Q& q, R& r, const Line& line);

		template<class Db, class R>
		void insert_content(Db& db, R& r, const Line& line);

		template<class Db>
		void insert_book_info_and_set_id(Db& db, Book& book);

		template<class Db>
		void insert_project_info_and_set_id(Db& db, Project& view);

		template<class Db, class P, class Q>
		void select_pages(Db& db, const BookBuilder& builder, P& p, Q& q);

		template<class Db, class Q>
		void select_lines(Db& db, const PageBuilder& builder, Q& q);
	}

	template<class Db>
	UserSptr create_user(Db& db, const std::string& user,
			const std::string& pw,
			const std::string& email = "",
			const std::string& inst = "");

	template<class Db>
	void update_user(Db& db, const User& user);

	template<class Db>
	UserSptr select_user(Db& db, const std::string& name);

	template<class Db>
	UserSptr select_user(Db& db, int id);

	template<class Db>
	void delete_user(Db& db, const std::string& name);

	template<class Db>
	void delete_user(Db& db, int id);

	template<class Db>
	ProjectSptr insert_project(Db& db, Project& book);

	template<class Db>
	BookSptr insert_book(Db& db, Book& book);

	template<class Db>
	void update_book(Db& db, Project& view);

	struct ProjectEntry {
		bool is_book() const noexcept {return origin == projectid;}
		int origin, owner, projectid;
	};

	template<class Db>
	boost::optional<ProjectEntry> select_project_entry(Db& db, int projectid);

	template<class Db>
	void update_project_owner(Db& db, int projectid, int owner);

	template<class Db>
	BookSptr select_book(Db& db, const User& owner, int bookid);

	template<class Db>
	ProjectSptr select_project(Db& db, const Book& book, int projectid);

	template<class Db>
	std::vector<int> select_all_project_ids(Db& db, const User& owner);

	template<class Db>
	void update_line(Db& db, const Line& line);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::create_user(Db& db, const std::string& name, const std::string& pw,
			const std::string& email, const std::string& inst)
{
	using namespace sqlpp;
	auto password = Password::make(pw);
	tables::Users users;
	auto stmnt = insert_into(users).set(
		users.name = name,
		users.email = email,
		users.institute = inst,
		users.passwd = password.str()
	);
	auto id = db(stmnt);
	return std::make_shared<User>(name, password, email, inst, id);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::update_user(Db& db, const User& user)
{
	using namespace sqlpp;
	tables::Users users;
	// do not change user's name and id
	auto stmnt = update(users).set(
		users.email = user.email,
		users.institute = user.institute
	).where(users.userid == user.id());
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::select_user(Db& db, const std::string& name)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmnt);
	if (not res.empty())
		return pcw::detail::make_user(res.front());
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::select_user(Db& db, int id)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = select(all_of(users)).from(users).where(users.userid == id);
	auto res = db(stmnt);
	if (not res.empty())
		return pcw::detail::make_user(res.front());
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::delete_user(Db& db, const std::string& name)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = remove_from(users).where(users.name == name);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::delete_user(Db& db, int id)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = remove_from(users).where(users.userid == id);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::detail::insert_project_info_and_set_id(Db& db, Project& view)
{
	using namespace sqlpp;
	tables::Projects projects;
	auto id = db(insert_into(projects)
			.set(projects.origin = view.origin().id(),
				projects.owner = view.owner().id()));
	view.set_id(id);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::insert_project(Db& db, Project& view)
{
	using namespace sqlpp;
	detail::insert_project_info_and_set_id(db, view);

	tables::ProjectPages project_pages;
	auto stmnt = db.prepare(insert_into(project_pages)
			.set(project_pages.projectid = view.id(),
				project_pages.pageid = parameter(project_pages.pageid)));
	for (const auto& page: view) {
		assert(page);
		stmnt.params.pageid = page->id();
		db(stmnt);
	}
	return view.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::detail::insert_book_info_and_set_id(Db& db, Book& book)
{
	using namespace sqlpp;
	tables::Projects projects;
	auto id = db(insert_into(projects)
			.set(projects.origin = 0,
				projects.owner = book.owner().id()));
	book.set_id(id);
	db(update(projects).set(projects.origin = id)
			.where(projects.projectid == id));
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookSptr
pcw::insert_book(Db& db, Book& book)
{
	using namespace sqlpp;
	detail::insert_book_info_and_set_id(db, book);

	tables::Books books;
	auto stmnt = insert_into(books).set(
		books.author = book.author,
		books.title = book.title,
		books.directory = book.dir.string(),
		books.year = book.year,
		books.uri = book.uri,
		books.bookid = book.id(),
		books.description = book.description,
		books.lang = book.lang
	);
	db(stmnt);

	tables::Pages pages;
	auto p = db.prepare(insert_into(pages).set(
		pages.bookid = parameter(pages.bookid),
		pages.pageid = parameter(pages.pageid),
		pages.imagepath = parameter(pages.imagepath),
		pages.ocrpath = parameter(pages.ocrpath),
		pages.pleft = parameter(pages.pleft),
		pages.ptop = parameter(pages.ptop),
		pages.pright = parameter(pages.pright),
		pages.pbottom = parameter(pages.pbottom)
	));

	tables::Textlines textlines;
	auto q = db.prepare(insert_into(textlines).set(
		textlines.bookid = parameter(textlines.bookid),
		textlines.pageid = parameter(textlines.pageid),
		textlines.lineid = parameter(textlines.lineid),
		textlines.imagepath = parameter(textlines.imagepath),
		textlines.lleft = parameter(textlines.lleft),
		textlines.ltop = parameter(textlines.ltop),
		textlines.lright = parameter(textlines.lright),
		textlines.lbottom = parameter(textlines.lbottom)
	));
	tables::Contents contents;
	auto r = db.prepare(insert_into(contents).set(
		contents.bookid = parameter(contents.bookid),
		contents.pageid = parameter(contents.pageid),
		contents.lineid = parameter(contents.lineid),
		contents.seq = parameter(contents.seq),
		contents.ocr = parameter(contents.ocr),
		contents.cor = parameter(contents.cor),
		contents.cut = parameter(contents.cut),
		contents.conf = parameter(contents.conf)
	));
	for (const auto& page: book) {
		assert(page);
		detail::insert_page(db, p, q, r, *page);
	}
	return std::static_pointer_cast<Book>(book.shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class P, class Q, class R>
void
pcw::detail::insert_page(Db& db, P& p, Q& q, R& r, const Page& page)
{
	p.params.bookid = page.book().id();
	p.params.pageid = page.id();
	p.params.imagepath = page.img.string();
	p.params.ocrpath = page.ocr.string();
	p.params.pleft = page.box.left();
	p.params.ptop = page.box.top();
	p.params.pright = page.box.right();
	p.params.pbottom = page.box.bottom();
	db(p);
	for (const auto& line: page) {
		assert(line);
		detail::insert_line(db, q, r, *line);
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class Q, class R>
void
pcw::detail::insert_line(Db& db, Q& q, R& r, const Line& line)
{
	q.params.bookid = line.page().book().id();
	q.params.pageid = line.page().id();
	q.params.lineid = line.id();
	q.params.imagepath = line.img.string();
	q.params.lleft = line.box.left();
	q.params.ltop = line.box.top();
	q.params.lright = line.box.right();
	q.params.lbottom = line.box.bottom();
	db(q);
	insert_content(db, r, line);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class R>
void
pcw::detail::insert_content(Db& db, R& r, const Line& line)
{
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
template<class Db>
void
pcw::update_book(Db& db, Project& view)
{
	using namespace sqlpp;
	tables::Books books;
	auto stmnt = update(books).set(
		books.author = view.origin().author,
		books.title = view.origin().title,
		books.directory = view.origin().dir.string(),
		books.year = view.origin().year,
		books.uri = view.origin().uri,
		books.description = view.origin().description,
		books.lang = view.origin().lang
	).where(books.bookid == view.origin().id());
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
boost::optional<pcw::ProjectEntry>
pcw::select_project_entry(Db& db, int projectid)
{
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = select(projects.origin, projects.owner)
		.from(projects)
		.where(projects.projectid == projectid);
	auto res = db(stmnt);

	if (not res.empty())
		return ProjectEntry{
			static_cast<int>(res.front().origin),
			static_cast<int>(res.front().owner),
			projectid
		};
	return {};
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::update_project_owner(Db& db, int projectid, int owner)
{
	using namespace sqlpp;
	tables::Projects projects;
	auto stmnt = update(projects)
		.set(projects.owner = owner)
		.where(projects.projectid == projectid);
	db(stmnt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookSptr
pcw::select_book(Db& db, const User& owner, int bookid)
{
	using namespace sqlpp;
	tables::Projects projects;
	tables::Books books;

	auto stmnt = select(all_of(books))
		.from(books.join(projects).on(books.bookid == projects.origin))
		.where(books.bookid == bookid and projects.owner == owner.id());
	auto res = db(stmnt);

	if (res.empty())
		return nullptr;

	tables::Pages pages;
	tables::Textlines textlines;
	tables::Contents contents;
	auto p = select(all_of(pages))
		.from(pages)
		.where(pages.bookid == bookid)
		.order_by(pages.pageid.asc());
	auto q = db.prepare(
		select(textlines.bookid, textlines.pageid, textlines.lineid,
			textlines.lleft, textlines.ltop, textlines.lright,
			textlines.lbottom, textlines.imagepath,
			contents.ocr, contents.cor, contents.cut, contents.conf)
		.from(textlines.join(contents)
			.on(textlines.bookid == contents.bookid and
				textlines.pageid == contents.pageid and
				textlines.lineid == contents.lineid))
		.where(textlines.bookid == bookid and
			textlines.pageid == parameter(textlines.pageid))
		.order_by(textlines.lineid.asc(), contents.seq.asc()));

	BookBuilder builder;
	detail::set_if_not_null(res.front().description, [&](const auto& d) {
		builder.set_description(d);
	});
	detail::set_if_not_null(res.front().uri, [&](const auto& u) {
		builder.set_uri(u);
	});
	detail::set_if_not_null(res.front().year, [&](const auto& y) {
		builder.set_year(y);
	});
	detail::set_if_not_null(res.front().author, [&](const auto& a) {
		builder.set_author(a);
	});
	detail::set_if_not_null(res.front().title, [&](const auto& t) {
		builder.set_title(t);
	});
	detail::set_if_not_null(res.front().bookid, [&](const auto& id) {
		builder.set_id(id);
	});
	detail::set_if_not_null(res.front().lang, [&](const auto& l) {
		builder.set_language(l);
	});
	builder.set_owner(owner);
	detail::select_pages(db, builder, p, q);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class P, class Q>
void
pcw::detail::select_pages(Db& db, const BookBuilder& builder, P& p, Q& q)
{
	using namespace sqlpp;

	PageBuilder pbuilder;
	for (const auto& row: db(p)) {
		pbuilder.reset();
		detail::set_if_not_null(row.imagepath, [&](const auto& p) {
			pbuilder.set_image_path(Path(p));
		});
		detail::set_if_not_null(row.ocrpath, [&](const auto& p) {
			pbuilder.set_ocr_path(Path(p));
		});
		pbuilder.set_box({
			static_cast<int>(row.pleft),
			static_cast<int>(row.ptop),
			static_cast<int>(row.pright),
			static_cast<int>(row.pbottom)
		});
		q.params.pageid = row.pageid;
		detail::select_lines(db, pbuilder, q);
		builder.append(*pbuilder.build());
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class Q>
void
pcw::detail::select_lines(Db& db, const PageBuilder& builder, Q& q)
{
	LineBuilder lbuilder;
	for (const auto& row: db(q)) {
		lbuilder.reset();
		detail::set_if_not_null(row.imagepath, [&](const auto& p) {
			lbuilder.set_image_path(Path(p));
		});
		lbuilder.set_box({
			static_cast<int>(row.lleft),
			static_cast<int>(row.ltop),
			static_cast<int>(row.lright),
			static_cast<int>(row.lbottom)
		});
		lbuilder.append(
			static_cast<wchar_t>(row.ocr),
			static_cast<wchar_t>(row.cor),
			row.cut,
			row.conf
		);
		builder.append(*lbuilder.build());
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::ProjectSptr
pcw::select_project(Db& db, const Book& book, int projectid)
{
	using namespace sqlpp;

	tables::ProjectPages project_pages;
	auto stmnt = select(project_pages.pageid)
		.from(project_pages)
		.where(project_pages.projectid == projectid);
	ProjectBuilder builder;
	for (const auto& row: db(stmnt)) {
		builder.add_page(row.pageid);
	}
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
std::vector<int>
pcw::select_all_project_ids(Db& db, const User& owner)
{
	using namespace sqlpp;
	std::vector<int> ids;
	tables::Projects projects;
	auto stmnt = select(projects.projectid)
		.from(projects)
		.where(projects.owner == owner.id() or projects.owner == 0);
	for (const auto& row: db(stmnt))
		ids.push_back(row.projectid);
	return ids;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::update_line(Db& db, const Line& line)
{
	using namespace sqlpp;
	tables::Contents contents;
	auto remove = remove_from(contents)
		.where(contents.bookid == line.page().book().id() and
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
		contents.conf = parameter(contents.conf)
	));
	db.run(remove);
	detail::insert_content(db, insert, line);
}

#endif // pcw_NewDatabase_hpp__
