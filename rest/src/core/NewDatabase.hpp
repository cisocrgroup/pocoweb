#ifndef pcw_NewDatabase_hpp__
#define pcw_NewDatabase_hpp__

#include <memory>

namespace pcw {
	class User;
	using UserSptr = std::shared_ptr<User>;
	class Book;
	using BookSptr = std::shared_ptr<Book>;
	class BookView;
	using BookViewSptr = std::shared_ptr<BookView>;

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
	auto stmt = insert_into(users).set(
		users.name = name,
		users.email = email,
		users.institute = inst,
		users.passwd = password.str()
	);
	auto id = db(stmt);
	return std::make_shared<User>(name, email, inst, id);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::login_user(Db& db, const std::string& name, const std::string& pw)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmt = select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmt);
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
	auto stmt = update(users).set(
		users.email = user.email,
		users.institute = user.institute
	).where(users.userid == user.id());
	db(stmt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
pcw::UserSptr
pcw::select_user(Db& db, const std::string& name)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmt = select(all_of(users)).from(users).where(users.name == name);
	auto res = db(stmt);
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
	auto stmt = select(all_of(users)).from(users).where(users.userid == id);
	auto res = db(stmt);
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
	auto stmt = remove_from(users).where(users.name == name);
	db(stmt);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db>
void
pcw::delete_user(Db& db, int id)
{
	using namespace sqlpp;
	tables::Users users;
	auto stmt = remove_from(users).where(users.userid == id);
	db(stmt);
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
		auto stmt1 = insert_into(projects).set(
			projects.origin = 0,
			projects.owner = view.owner().id()
		);
		id = db(stmt1);
		auto stmt2 = update(projects).set(
			projects.origin = id
		).where(projects.projectid == id);
		db(stmt2);
	} else {
		auto stmt = insert_into(projects).set(
			projects.origin = view.origin().id(),
			projects.owner = view.owner().id()
		);
		id = db(stmt);
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
	auto stmt = insert_into(books).set(
		books.author = book.author,
		books.title = book.title,
		books.directory = book.dir.string(),
		books.year = book.year,
		books.uri = book.uri,
		books.bookid = book.id(),
		books.description = book.description,
		books.lang = book.lang
	);
	db(stmt);
	return std::static_pointer_cast<Book>(book.shared_from_this());
}

#endif // pcw_NewDatabase_hpp__
