#include <crow.h>
#include <cassert>
#include <unordered_set>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <crow/logging.h>
#include "Error.hpp"
#include "Book.hpp"
#include "AppCache.hpp"
#include "Page.hpp"
#include "Package.hpp"
#include "Line.hpp"
#include "Config.hpp"
#include "Sessions.hpp"
#include "Database.hpp"
#include "User.hpp"
#include "db.hpp"

static const int SHA2_HASH_SIZE = 256;

using namespace pcw;
// use macro to get the line of missing session locks
#define check_session_lock() \
{ \
	assert(this->session_); \
	if (this->session_->mutex.try_lock()) { \
		this->session_->mutex.unlock(); \
		THROW(Error, "(Database) Current session is not locked"); \
	} \
}

////////////////////////////////////////////////////////////////////////////////
Database::Database(SessionPtr session, ConfigPtr config, CachePtr cache)
	: scope_guard_()
	, session_(std::move(session))
	, config_(std::move(config))
	, cache_(std::move(cache))
{
	assert(session_);
	assert(config_);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::insert_user(const std::string& name, const std::string& pass) const
{
	static const char *sql = "INSERT INTO users (name, passwd) "
				 "VALUES (?, SHA2(?, ?))"
				 ";";

	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->setString(2, pass);
	s->setInt(3, SHA2_HASH_SIZE);
	s->executeUpdate();

	const int userid = last_insert_id(*conn);
	if (not userid)
		return nullptr;
	return put_cache(std::make_shared<User>(name, "", "", userid));
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::authenticate(const std::string& name, const std::string& pass) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ? and passwd = SHA2(?,?)"
				 ";";
	check_session_lock();
	auto conn = connection();
	assert(conn);

	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->setString(2, pass);
	s->setInt(3, SHA2_HASH_SIZE);
	return put_cache(
		get_user_from_result_set(ResultSetPtr{s->executeQuery()})
	);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(const std::string& name) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	return cached_select_user(name, *conn);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(const std::string& name, sql::Connection& conn) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ?;";
	PreparedStatementPtr s(conn.prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	return get_user_from_result_set(ResultSetPtr{s->executeQuery()});
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(int userid) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	return cached_select_user(userid, *conn);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(int userid, sql::Connection& conn) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE userid = ?"
				 ";";
	PreparedStatementPtr s(conn.prepareStatement(sql));
	assert(s);
	s->setInt(1, userid);
	return get_user_from_result_set(ResultSetPtr{s->executeQuery()});
}

////////////////////////////////////////////////////////////////////////////////
void
Database::update_user(const User& user) const
{
	// just update email and institute; not name or userid
	static const char *sql = "UPDATE users "
				 "SET institute=?,email=? "
				 "WHERE userid = ? "
				 ";";
	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, user.institute);
	s->setString(2, user.email);
	s->setInt(3, user.id());
	s->execute();
}

////////////////////////////////////////////////////////////////////////////////
void
Database::delete_user(const std::string& name) const
{
	static const char *sql = "DELETE FROM users "
				 "WHERE name = ?"
				 ";";
	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->execute();
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::get_user_from_result_set(ResultSetPtr res)
{
	if (not res or not res->next())
		return nullptr;
	return std::make_shared<User>(
		res->getString("name"),
		res->getString("email"),
		res->getString("institute"),
		res->getInt("userid")
	);
}

////////////////////////////////////////////////////////////////////////////////
void
Database::update_line(const Line& line) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	update_line(line, *conn);
}

////////////////////////////////////////////////////////////////////////////////
void
Database::update_line(const Line& line, sql::Connection& conn) const
{
	static const char* sql =
		"DELETE FROM contents "
		"WHERE bookid = ? AND pageid = ? AND lineid = ?;";
	static const char* tql =
		"INSERT INTO contents "
		"(bookid, pageid, lineid, seq, ocr, cor, cut, conf) "
		"VALUES (?,?,?,?,?,?,?,?);";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	const auto lineid = line.id();
	const auto pageid = line.page()->id();
	const auto bookid = line.page()->book()->id();
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, lineid);
	s->executeUpdate();

	PreparedStatementPtr t{conn.prepareStatement(tql)};
	t->setInt(1, bookid);
	t->setInt(2, pageid);
	t->setInt(3, lineid);
	for (size_t i = 0; i < line.size(); ++i) {
		t->setInt(4, static_cast<int>(i));
		t->setInt(5, line[i].ocr);
		t->setInt(6, line[i].cor);
		t->setInt(7, line[i].cut);
		t->setDouble(8, line[i].conf);
		t->executeUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::insert_project(BookView& project) const
{
	static const char *sql = "INSERT INTO projects "
				 "(origin,owner) "
				 "VALUES (?,?);";
	static const char *tql = "INSERT INTO project_pages "
				 "(projectid,pageid) "
				 "VALUES (?,?);";
	check_session_lock();
	auto conn = connection();
	assert(conn);

	PreparedStatementPtr s{conn->prepareStatement(sql)};
	assert(s);
	s->setInt(1, project.origin().id());
	s->setInt(2, project.owner().id());
	s->executeUpdate();
	const auto projectid = last_insert_id(*conn);
	project.set_id(projectid);

	PreparedStatementPtr t{conn->prepareStatement(tql)};
	assert(t);
	t->setInt(1, project.id());
	std::for_each(begin(project), end(project), [&t](const auto& page) {
		assert(page);
		t->setInt(2, page->id());
		t->executeUpdate();
	});
	return put_cache(project.shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
void
Database::update_book(const BookView& view) const
{
	static const char *sql =
		"UPDATE books "
		"SET author=?,title=?,year=?,uri=?,description=? "
		"WHERE bookid=?";
	check_session_lock();
	auto conn = connection();
	PreparedStatementPtr s{conn->prepareStatement(sql)};
	assert(s);
	s->setString(1, view.origin().author);
	s->setString(2, view.origin().title);
	s->setInt(3, view.origin().year);
	s->setString(4, view.origin().uri);
	s->setString(5, view.origin().description);
	s->setInt(6, view.origin().id());
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
Database::insert_book(Book& book) const
{
	static const char *sql =
		"INSERT INTO books "
		"(author, title, directory, year, uri, bookid, description) "
		"VALUES (?,?,?,?,?,?,?);";
	check_session_lock();
	auto conn = connection();
	assert(conn);

	const auto projectid = insert_book_project(book, *conn);

	PreparedStatementPtr s{conn->prepareStatement(sql)};
	assert(s);
	s->setString(1, book.author);
	s->setString(2, book.title);
	s->setString(3, book.dir.string());
	s->setInt(4, book.year);
	s->setString(5, book.uri);
	s->setInt(6, projectid);
	s->setString(7, book.description);
	s->executeUpdate();
	book.set_id(last_insert_id(*conn));
	if (not book.id())
		return nullptr;
	for (const auto& page: book) {
		assert(page);
		insert_page(*page, *conn);
	}
	update_project_origin_id(projectid, *conn);
	return std::static_pointer_cast<Book>(
		put_cache(book.shared_from_this())
	);
}

////////////////////////////////////////////////////////////////////////////////
int
Database::insert_book_project(const BookView& project, sql::Connection& conn) const
{
	static const char *sql = "INSERT INTO projects (origin, owner) "
				 "VALUES (0,?);";
	assert(project.is_book());
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, project.owner().id());
	s->executeUpdate();
	return last_insert_id(conn);
}

////////////////////////////////////////////////////////////////////////////////
void
Database::update_project_origin_id(int id, sql::Connection& conn) const
{
	static const char *sql = "UPDATE projects "
				 "SET origin = ? "
				 "WHERE projectid = ?;";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, id);
	s->setInt(2, id);
	s->executeUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void
Database::insert_page(const Page& page, sql::Connection& conn) const
{
	static const char* sql =
		"INSERT INTO pages "
		"(bookid, pageid, imagepath, ocrpath, pleft, ptop, pright, pbottom) "
		"VALUES (?,?,?,?,?,?,?,?);";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, page.book()->id());
	s->setInt(2, page.id());
	s->setString(3, page.img.string());
	s->setString(4, page.ocr.string());
	s->setInt(5, page.box.left());
	s->setInt(6, page.box.top());
	s->setInt(7, page.box.right());
	s->setInt(8, page.box.bottom());
	s->executeUpdate();
	for (const auto& line: page)
		insert_line(*line, conn);
}

////////////////////////////////////////////////////////////////////////////////
void
Database::insert_line(const Line& line, sql::Connection& conn) const
{
	static const char* sql =
		"INSERT INTO textlines "
		"(bookid, pageid, lineid, imagepath, lleft, ltop, lright, lbottom) "
		"VALUES (?,?,?,?,?,?,?,?);";
	static const char* tql =
		"INSERT INTO contents "
		"(bookid, pageid, lineid, seq, ocr, cor, cut, conf) "
		"VALUES (?,?,?,?,?,?,?,?);";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	const auto pageid = line.page()->id();
	const auto bookid = line.page()->book()->id();
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, line.id());
	s->setString(4, line.img.string());
	s->setInt(5, line.box.left());
	s->setInt(6, line.box.top());
	s->setInt(7, line.box.right());
	s->setInt(8, line.box.bottom());
	s->executeUpdate();

	PreparedStatementPtr t{conn.prepareStatement(tql)};
	assert(t);
	for (auto i = 0U; i < line.size(); ++i) {
		t->setInt(1, bookid);
		t->setInt(2, pageid);
		t->setInt(3, line.id());
		t->setInt(4, static_cast<int>(i));
		t->setInt(5, line[i].ocr);
		t->setBoolean(6, line[i].cor);
		t->setInt(7, line[i].cut);
		t->setDouble(8, line[i].conf);
		// t->setInt(8, line[i].ocr); // TODO update table;
		t->executeUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::select_project(int projectid) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	return cached_select_project(projectid, *conn);
}

////////////////////////////////////////////////////////////////////////////////
std::vector<BookViewPtr>
Database::select_all_projects(const User& user) const
{
	static const char *sql = "SELECT projectid "
				 "FROM projects "
				 "WHERE owner = ? OR owner = 0;";
	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s{conn->prepareStatement(sql)};
	assert(s);
	s->setInt(1, user.id());
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	std::vector<BookViewPtr> projects;
	while (res->next()) {
		const auto prid = res->getInt(1);
		projects.push_back(cached_select_project(prid, *conn));
	}
	return projects;
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::select_project(int projectid, sql::Connection& conn) const
{
	static const char *sql =
		"SELECT origin,owner FROM projects WHERE projectid = ?;";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, projectid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	if (not res->next())
		return nullptr;
	auto origin = res->getInt(1);
	auto owner = res->getInt(2);
	if (not origin or not owner) // TODO throw?
		return nullptr;

	// return book or subproject
	return origin == projectid ?
		select_book(origin, owner, conn) :
		select_subproject(projectid, origin, owner, conn);
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::select_subproject(int projectid, int origin, int owner,
		sql::Connection& conn) const
{
	// it is save to use the cache here
	auto book = cached_select_project(origin, conn);
	if (not book) // TODO throw?
		return nullptr;
	assert(book->is_book()); // origin must be a book!
	return select_subproject(projectid, owner, book->origin(), conn);
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::select_subproject(int projectid, int owner, const Book& origin,
		sql::Connection& conn
) const
{
	static const char *sql = "SELECT pageid FROM project_pages "
				 "WHERE projectid = ?;";
	// it is save to use cache here
	auto ownerptr = cached_select_user(owner, conn);
	if (not ownerptr)
		THROW(Error, "Invalid user id: ", owner);

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, projectid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	std::unordered_set<int> ids; // pageids could be in sorted order
	while (res->next()) {
		ids.insert(res->getInt(1));
	}
	auto project = std::make_shared<Package>(projectid, *ownerptr, origin);
	std::copy_if(begin(origin), end(origin), std::back_inserter(*project),
		[&ids](const auto& page) {
			assert(page);
			return ids.count(page->id());
	});
	return project;
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
Database::select_book(int bookid, int owner, sql::Connection& conn) const
{
	static const char *sql = "SELECT * FROM books WHERE bookid = ?;";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, bookid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	if (not res->next())
		return nullptr;

	auto book = std::make_shared<Book>(bookid);
	book->description = res->getString("description");
	book->uri = res->getString("uri");
	book->dir = res->getString("directory");
	book->title = res->getString("title");
	book->author = res->getString("author");
	book->year = res->getInt("year");

	// it is save to use cache here
	auto ownerptr = cached_select_user(owner, conn);
	if (not ownerptr) // TODO throw?
		return nullptr;
	book->set_owner(*ownerptr);
	select_all_pages(*book, conn);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
void
Database::select_all_pages(Book& book, sql::Connection& conn) const
{
	CROW_LOG_DEBUG << "(Database) Start: select all pages";
	static const char *sql = "SELECT * FROM pages "
				 "WHERE bookid = ? "
				 "ORDER BY pageid;";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	s->setInt(1, book.id());
	ResultSetPtr res{s->executeQuery()};
	assert(res);

	while (res->next()) {
		const int id = res->getInt("pageid");
		const int l = res->getInt("pleft");
		const int r = res->getInt("pright");
		const int t = res->getInt("ptop");
		const int b = res->getInt("pbottom");
		auto page = std::make_shared<Page>(id, Box{l, t, r, b});
		assert(res->getInt("bookid") == book.id());
		page->img = res->getString("imagepath");
		page->ocr = res->getString("ocrpath");
		// first insert the page into books; then load lines
		// otherwise page->book() is not set!
		book.push_back(page);
		select_all_lines(*book.back(), conn);
	}
	CROW_LOG_DEBUG << "(Database) End: select all pages";
}

////////////////////////////////////////////////////////////////////////////////
void
Database::select_all_lines(Page& page, sql::Connection& conn) const
{
	static const char *sql = "SELECT l.bookid,l.pageid,l.lineid,"
				 "       l.lleft,l.lright,l.ltop,l.lbottom,"
				 "       l.imagepath,"
				 "       c.ocr,c.cor,c.cut,c.conf "
			  	 "FROM textlines AS l JOIN contents AS c "
				 "ON (l.bookid=c.bookid AND"
				 "    l.pageid=c.pageid AND"
				 "    l.lineid=c.lineid) "
				 "WHERE l.bookid=? AND l.pageid=? "
				 "ORDER BY l.lineid,seq;";
	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	const int bookid = page.book()->id();
	const int pageid = page.id();
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);
	LinePtr line = nullptr;
	while (res->next()) {
		assert(res->getInt(1) == bookid);
		assert(res->getInt(2) == pageid);

		const int id = res->getInt(3);
		const int l = res->getInt(4);
		const int r = res->getInt(5);
		const int t = res->getInt(6);
		const int b = res->getInt(7);

		// finished with current line
		if (not line or line->id() != id) {
			if (line)
				page.push_back(std::move(line));
			line = std::make_shared<Line>(id, Box{l, t, r, b});
			line->img = res->getString(8);
		}
		const wchar_t ocr = res->getInt(9);
		const wchar_t cor = res->getInt(10);
		const auto cut = res->getInt(11);
		const auto conf = res->getDouble(12);
		line->append(ocr, cor, cut, conf);
	}
	// insert last line
	if (line)
		page.push_back(std::move(line));
}

////////////////////////////////////////////////////////////////////////////////
bool
Database::autocommit() const noexcept
{
	return not scope_guard_;
}

////////////////////////////////////////////////////////////////////////////////
void
Database::set_autocommit(bool ac)
{
	check_session_lock();
	if (ac) {
		scope_guard_ = boost::none;
	} else {
		scope_guard_.emplace([this](){
			assert(session_);
			assert(session_->connection);
			session_->connection->rollback();
		});
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Database::commit()
{
	CROW_LOG_INFO << "(Database) Committing data";
	check_session_lock();
	if (scope_guard_) {
		scope_guard_->dismiss();
		assert(session_);
		assert(session_->connection);
		session_->connection->commit();
	} else {
		CROW_LOG_ERROR << "(Database) call to commit() in auto commit mode";
	}
}

////////////////////////////////////////////////////////////////////////////////
int
Database::last_insert_id(sql::Connection& conn) const
{
	StatementPtr s{conn.createStatement()};
	assert(s);
	ResultSetPtr res{s->executeQuery("select last_insert_id()")};
	assert(res);
	return res->next() ? res->getInt(1) : 0;
}

////////////////////////////////////////////////////////////////////////////////
sql::Connection*
Database::connection() const
{
	assert(config_);
	assert(session_);
	sql::Connection* conn = nullptr;
	assert(not session_->mutex.try_lock()); // session has to be locked already

	if (session_->connection and session_->connection->isValid()) {
		conn = session_->connection.get();
	} else if (session_->connection and session_->connection->reconnect()) {
		conn = session_->connection.get();
	} else { // new connection or could not reconnect
		session_->connection = connect(*config_);
		assert(session_->connection);
		conn = session_->connection.get();
	}

	assert(conn);
	// if scope_guard is not set, use autocommit mode
	if (not scope_guard_)
		conn->setAutoCommit(true);
	else
		conn->setAutoCommit(false);
	return conn;
}
////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::cached_select_user(const std::string& name, sql::Connection& conn) const
{
	auto get_user = [this,&conn](const std::string& name) {
		CROW_LOG_INFO << "(Database) Loading not cached user: " << name;
		return select_user(name, conn);
	};
	return cache_ ? cache_->user.get(name, get_user) : get_user(name);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::cached_select_user(int userid, sql::Connection& conn) const
{
	auto get_user = [this,&conn](int userid) {
		CROW_LOG_INFO << "(Database) Loading not cached user: " << userid;
		auto user = select_user(userid, conn);
		if (user)
			CROW_LOG_INFO << "(Database) Loaded user: " << *user;
		return user;
	};
	return cache_ ? cache_->user.get(userid, get_user) : get_user(userid);
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::cached_select_project(int prid, sql::Connection& conn) const
{
	auto get_project = [this,&conn](int prid) {
		CROW_LOG_INFO << "(Database) Loading not cached project: " << prid;
		auto proj = select_project(prid, conn);
		if (proj)
			CROW_LOG_INFO << "(Database) Loaded project: " << *proj;
		return proj;

	};
	return cache_ ?
		cache_->project.get(prid, get_project) :
		get_project(prid);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::put_cache(UserPtr user) const
{
	if (user and cache_) {
		CROW_LOG_INFO << "(Database) Caching User " << *user;
		cache_->user.put(user);
	}
	return user;
}

////////////////////////////////////////////////////////////////////////////////
BookViewPtr
Database::put_cache(BookViewPtr proj) const
{
	if (proj and cache_) {
		CROW_LOG_INFO << "(Database) Caching BookView " << *proj;
		cache_->project.put(proj);
	}
	return proj;
}
