#include <crow.h>
#include <cassert>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <crow/logging.h>
#include "Book.hpp"
#include "Page.hpp"
#include "Line.hpp"
#include "BookDir.hpp"
#include "Config.hpp"
#include "Sessions.hpp"
#include "Database.hpp"
#include "User.hpp"
#include "db.hpp"

static const int SHA2_HASH_SIZE = 256;

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Database::Database(SessionPtr session, ConfigPtr config)
	: scope_guard_()
	, session_(std::move(session))
	, config_(std::move(config))
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

	const int user_id = last_insert_id(*conn);
	return user_id ?
		std::make_shared<User>(name, "", "", user_id) :
		nullptr;
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
	return get_user_from_result_set(ResultSetPtr{s->executeQuery()});
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(const std::string& name) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	return select_user(name, *conn);
}

////////////////////////////////////////////////////////////////////////////////
UserPtr
Database::select_user(const std::string& name, sql::Connection& conn) const
{
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ?"
				 ";";
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
	return select_user(userid, *conn);
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
BookPtr 
Database::insert_book(Book& book) const
{
	static const char *sql = "INSERT INTO books "
				 "(author, title, directory, year, uri, bookid, description) "
				  "VALUES (?,?,?,?,?,?,?);"; 
	check_session_lock();
	auto conn = connection();
	assert(conn);

	book.set_owner(*session_->user);
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
	return std::static_pointer_cast<Book>(book.shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
int
Database::insert_book_project(const Project& project, sql::Connection& conn) const
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
	s->setInt(2, page.id);
	s->setString(3, page.img.string());
	s->setString(4, page.ocr.string());
	s->setInt(5, page.box.left());
	s->setInt(6, page.box.top());
	s->setInt(7, page.box.right());
	s->setInt(8, page.box.bottom());
	s->executeUpdate();
	for (const auto& line: page) 
		insert_line(line, conn);
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
		"(bookid, pageid, lineid, seq, letter, cut, conf) "
		"VALUES (?,?,?,?,?,?,?);";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	const auto pageid = line.page()->id;
	const auto bookid = line.page()->book()->id();
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	s->setInt(3, line.id);
	s->setString(4, line.img.string());
	s->setInt(5, line.box.left());
	s->setInt(6, line.box.top());
	s->setInt(7, line.box.right());
	s->setInt(8, line.box.bottom());
	s->executeUpdate();

	PreparedStatementPtr t{conn.prepareStatement(tql)};
	assert(t);
	for (auto i = 0U; i < line.wstring().size(); ++i) {
		t->setInt(1, bookid);
		t->setInt(2, pageid);
		t->setInt(3, line.id);
		t->setInt(4, static_cast<int>(i));
		t->setInt(5, line.wstring().at(i));
		t->setInt(6, line.cuts().at(i));
		t->setDouble(7, line.confidences().at(i));
		t->executeUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
ProjectPtr
Database::select_project(int projectid) const
{
	check_session_lock();
	auto conn = connection();
	assert(conn);
	// TODO Implement this
	return select_book(projectid, *conn);
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
Database::select_book(int bookid, sql::Connection& conn) const
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
	book->set_owner(*select_user(res->getInt("owner")));
	select_all_pages(*book, conn);
	return book;
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::select_all_pages(Book& book, sql::Connection& conn) const
{
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
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::select_all_lines(Page& page, sql::Connection& conn) const
{
	static const char *sql = "SELECT * FROM textlines "
				 "WHERE bookid = ? AND pageid = ? "
				 "ORDER BY lineid;";
	static const char *cql = "SELECT * FROM contents "
				 "WHERE bookid = ? AND pageid = ? AND lineid = ? "
				 "ORDER BY seq;";

	PreparedStatementPtr s{conn.prepareStatement(sql)};
	assert(s);
	const int bookid = page.book()->id();
	const int pageid = page.id;
	s->setInt(1, bookid);
	s->setInt(2, pageid);
	ResultSetPtr res{s->executeQuery()};
	assert(res);

	PreparedStatementPtr c{conn.prepareStatement(cql)};
	assert(c);
	c->setInt(1, bookid);
	c->setInt(2, pageid);

	while (res->next()) {
		assert(res->getInt("bookid") == bookid);
		assert(res->getInt("pageid") == pageid);

		const int id = res->getInt("lineid");
		const int l = res->getInt("lleft");
		const int r = res->getInt("lright");
		const int t = res->getInt("ltop");
		const int b = res->getInt("lbottom");
		Line line(id, {l, t, r, b});
		line.img = res->getString("imagepath");

		c->setInt(3, id);
		ResultSetPtr contents{c->executeQuery()};
		assert(contents);
		
		while (contents->next()) {
			assert(contents->getInt("bookid") == bookid);
			assert(contents->getInt("pageid") == pageid);
			assert(contents->getInt("lineid") == line.id);
			
			const wchar_t l = contents->getInt("letter");	
			const auto r = contents->getInt("cut");	
			const auto c = contents->getDouble("conf");	
			line.append(l, r, c);
		}
		page.push_back(std::move(line));
	}
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
void 
Database::check_session_lock() const
{
	assert(session_);
	if (session_->mutex.try_lock()) {
		session_->mutex.unlock(); // unlock
		throw std::logic_error("(Database) Current session is not locked");
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
