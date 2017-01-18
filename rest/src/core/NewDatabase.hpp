#ifndef pcw_NewDatabase_hpp__
#define pcw_NewDatabase_hpp__

#include <boost/optional.hpp>
#include <memory>
#include "Tables.h"

namespace pcw {
	class User;
	using UserSptr = std::shared_ptr<User>;
	class Book;
	using BookSptr = std::shared_ptr<Book>;
	class BookView;
	using BookViewSptr = std::shared_ptr<BookView>;
	class Page;
	class Line;
	class BookBuilder;
	class LineBuilder;
	class PageBuilder;

	namespace detail {
		template<class U>
		UserSptr make_user(const U& users) {
			return std::make_shared<User>(
				users.name,
				users.email,
				users.institute,
				users.userid
			);
		}
		template<class Db, class P, class Q, class R>
		void insert_page(Db& db, P& p, Q& q, R& r, const Page& page);

		template<class Db, class Q, class R>
		void insert_line(Db& db, Q& q, R& r, const Line& line);

		template<class Db, class P, class Q, class R>
		void select_pages(Db& db, const BookBuilder& builder, P& p, Q& q, R& r);

		template<class Db, class Q, class R>
		void select_lines(Db& db, const PageBuilder& builder, Q& q, R& r);
	}

	template<class Db>
	UserSptr create_user(Db& db, const std::string& user,
			const std::string& pw,
			const std::string& email = "",
			const std::string& inst = "");
	template<class Db>
	UserSptr login_user(Db& db, const std::string& user, const std::string& pw);

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
	BookViewSptr insert_project(Db& db, BookView& book);

	template<class Db>
	BookSptr insert_book(Db& db, Book& book);

	template<class Db>
	void update_book(Db& db, BookView& view);

	struct ProjectEntry {
		bool is_book() const noexcept {return origin == projectid;}
		int origin, owner, projectid;
	};

	template<class Db>
	boost::optional<ProjectEntry> select_project(Db& db, int projectid);

	template<class Db>
	void update_project_owner(Db& db, int projectid, int owner);

	template<class Db>
	BookSptr select_book(Db& db, const User& owner, int bookid);
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
	return std::make_shared<User>(name, email, inst, id);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::login_user(Db& db, const std::string& name, const std::string& pw)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmnt = select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmnt);
	if (not res.empty()) {
		Password password(res.front().passwd);
		if (password.authenticate(pw)) {
			return detail::make_user(res.front());
		}
	}
	return nullptr;
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
pcw::BookViewSptr
pcw::insert_project(Db& db, BookView& view)
{
	using namespace sqlpp;
	tables::Projects projects;
	int id;
	if (view.is_book()) {
		auto stmnt1 = insert_into(projects).set(
			projects.origin = 0,
			projects.owner = view.owner().id()
		);
		id = db(stmnt1);
		auto stmnt2 = update(projects).set(
			projects.origin = id
		).where(projects.projectid == id);
		db(stmnt2);
	} else {
		auto stmnt = insert_into(projects).set(
			projects.origin = view.origin().id(),
			projects.owner = view.owner().id()
		);
		id = db(stmnt);
	}
	view.set_id(id);
	return view.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::BookSptr
pcw::insert_book(Db& db, Book& book)
{
	using namespace sqlpp;
	tables::Books books;
	insert_project(db, book); // sets bookid
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
pcw::update_book(Db& db, BookView& view)
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
pcw::select_project(Db& db, int projectid)
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
	auto p = select(all_of(pages))
		.from(pages)
		.where(pages.bookid == bookid)
		.order_by(pages.pageid.asc());

	tables::Textlines textlines;
	auto q = db.prepare(
			select(all_of(textlines))
			.from(textlines)
			.where(textlines.bookid == bookid and
				textlines.pageid == parameter(textlines.pageid))
			.order_by(textlines.lineid.asc())
	);

	tables::Contents contents;
	auto r = db.prepare(
			select(all_of(contents))
			.from(contents)
			.where(contents.bookid == bookid and
				contents.pageid == parameter(contents.pageid) and
				contents.lineid == parameter(contents.lineid))
			.order_by(contents.seq.asc())
	);

	BookBuilder builder;
	builder.set_description(res.front().description);
	builder.set_uri(res.front().uri);
	builder.set_author(res.front().author);
	builder.set_title(res.front().title);
	builder.set_id(res.front().bookid);
	builder.set_language(res.front().lang);
	builder.set_owner(owner);
	builder.set_year(res.front().year);
	detail::select_pages(db, builder, p, q, r);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class P, class Q, class R>
void
pcw::detail::select_pages(Db& db, const BookBuilder& builder, P& p, Q& q, R& r)
{
	using namespace sqlpp;

	PageBuilder pbuilder;
	for (const auto& row: db(p)) {
		pbuilder.reset();
		pbuilder.set_image_path(Path(row.imagepath));
		pbuilder.set_ocr_path(Path(row.ocrpath));
		pbuilder.set_box({
			static_cast<int>(row.pleft),
			static_cast<int>(row.ptop),
			static_cast<int>(row.pright),
			static_cast<int>(row.pbottom)
		});
		q.params.pageid = row.pageid;
		r.params.pageid = row.pageid;
		detail::select_lines(db, pbuilder, q, r);
		builder.append(*pbuilder.build());
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class Q, class R>
void
pcw::detail::select_lines(Db& db, const PageBuilder& builder, Q& q, R& r)
{
	LineBuilder lbuilder;
	for (const auto& row: db(q)) {
		lbuilder.reset();
		lbuilder.set_image_path(Path(row.imagepath));
		lbuilder.set_box({
			static_cast<int>(row.lleft),
			static_cast<int>(row.ltop),
			static_cast<int>(row.lright),
			static_cast<int>(row.lbottom)
		});

		r.params.lineid = row.lineid;
		for (const auto& cont: db(r)) {
			lbuilder.append(
				static_cast<wchar_t>(cont.ocr),
				static_cast<wchar_t>(cont.cor),
				cont.cut,
				cont.conf
			);
		}
		builder.append(*lbuilder.build());
	}
}

#endif // pcw_NewDatabase_hpp__
