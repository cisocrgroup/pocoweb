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
	static const char *sql = "INSERT INTO users (name, passwd, active) "
				 "VALUES (?, SHA2(?, ?), ?)"
				 ";";
	
	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
	s->setString(2, pass);
	s->setInt(3, SHA2_HASH_SIZE);
	s->setBoolean(4, true);
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
	static const char *sql = "SELECT name,email,institute,userid "
				 "FROM users "
				 "WHERE name = ?"
				 ";";
	check_session_lock();
	auto conn = connection();
	assert(conn);
	PreparedStatementPtr s(conn->prepareStatement(sql));
	assert(s);
	s->setString(1, name);
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
	s->setInt(3, user.id);
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
				 "(owner, author, title, directory, year, uri) "
				  "VALUES (?, ?, ?, ?, ?, ?)"
				  ";";

	check_session_lock();
	auto conn = connection();
	assert(conn);

	PreparedStatementPtr bs{conn->prepareStatement(sql)};
	auto ps = make_insert_page_statement(*conn);
	auto ls = make_insert_line_statement(*conn);
	auto cs = make_insert_content_statement(*conn);
	assert(bs);
	assert(ps);
	assert(ls);
	assert(cs);

	bs->setInt(1, session_->user->id);
	bs->setString(2, book.author);
	bs->setString(3, book.title);
	bs->setString(4, book.dir.string());
	bs->setInt(5, book.year);
	bs->setString(6, book.uri);
	bs->executeUpdate();
	book.id = last_insert_id(*conn);
	if (not book.id)
		return nullptr;
	for (const auto& page: book) {
		assert(page);
		insert_page(*page, *ps, *ls, *cs);
	}

	book.owner = session_->user;
	return book.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::insert_page(
	const Page& page,
	sql::PreparedStatement& ps,
	sql::PreparedStatement& ls,
	sql::PreparedStatement& cs
) const {
	ps.setInt(1, page.book()->id);
	ps.setInt(2, page.id);
	ps.setString(3, page.img.string());
	ps.setString(4, page.ocr.string());
	ps.setInt(5, page.box.left());
	ps.setInt(6, page.box.top());
	ps.setInt(7, page.box.right());
	ps.setInt(8, page.box.bottom());
	ps.executeUpdate();
	for (const auto& line: page) 
		insert_line(line, ls, cs);
}

////////////////////////////////////////////////////////////////////////////////
void 
Database::insert_line(
	const Line& line, 
	sql::PreparedStatement& ls,
	sql::PreparedStatement& cs
) const {
	const auto pageid = line.page()->id;
	const auto bookid = line.page()->book()->id;

	ls.setInt(1, bookid);
	ls.setInt(2, pageid);
	ls.setInt(3, line.id);
	ls.setString(4, line.img.string());
	ls.setInt(5, line.box.left());
	ls.setInt(6, line.box.top());
	ls.setInt(7, line.box.right());
	ls.setInt(8, line.box.bottom());
	ls.executeUpdate();

	for (auto i = 0U; i < line.wstring().size(); ++i) {
		cs.setInt(1, bookid);
		cs.setInt(2, pageid);
		cs.setInt(3, line.id);
		cs.setInt(4, static_cast<int>(i));
		cs.setInt(5, line.wstring().at(i));
		cs.setInt(6, line.cuts().at(i));
		cs.setDouble(7, line.confidences().at(i));
		cs.executeUpdate();
	}
}

////////////////////////////////////////////////////////////////////////////////
PreparedStatementPtr 
Database::make_insert_page_statement(sql::Connection& conn)
{
	static const char* sql = 
		"INSERT INTO pages "
		"(bookid, pageid, imagepath, ocrpath, pleft, ptop, pright, pbottom) "
		"VALUES (?,?,?,?,?,?,?,?);";
	return PreparedStatementPtr{conn.prepareStatement(sql)};
}

////////////////////////////////////////////////////////////////////////////////
PreparedStatementPtr 
Database::make_insert_line_statement(sql::Connection& conn)
{
	static const char* sql = 
		"INSERT INTO textlines "
		"(bookid, pageid, lineid, imagepath, lleft, ltop, lright, lbottom) "
		"VALUES (?,?,?,?,?,?,?,?);";
	return PreparedStatementPtr{conn.prepareStatement(sql)};
}

////////////////////////////////////////////////////////////////////////////////
PreparedStatementPtr 
Database::make_insert_content_statement(sql::Connection& conn)
{
	static const char* sql = 
		"INSERT INTO contents "
		"(bookid, pageid, lineid, seq, letter, cut, conf) "
		"VALUES (?,?,?,?,?,?,?);";
	return PreparedStatementPtr{conn.prepareStatement(sql)};
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
