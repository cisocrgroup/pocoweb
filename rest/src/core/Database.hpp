#ifndef pcw_Database_hpp__
#define pcw_Database_hpp__

#include <boost/optional.hpp>
#include <memory>
#include "ScopeGuard.hpp"
#include "db.hpp"

namespace pcw {
	class Session;
	using SessionPtr = std::shared_ptr<Session>;
	class Config;
	using ConfigPtr = std::shared_ptr<Config>;
	class User;
	using UserPtr = std::shared_ptr<User>;
	using ResultSetPtr = std::unique_ptr<sql::ResultSet>;
	class Book;
	using BookPtr = std::shared_ptr<Book>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	class Line;

	class Database {
	public:
		Database(SessionPtr session, ConfigPtr config);
		
		void set_autocommit(bool ac = true);
		bool autocommit() const noexcept;
		void commit();
		Session& session() const noexcept {return *session_;}

		UserPtr insert_user(const std::string& name, const std::string& pass) const;
		UserPtr authenticate(const std::string& name, const std::string& pass) const;
		UserPtr select_user(const std::string& name) const;
		void update_user(const User& user) const;
		void delete_user(const std::string& name) const;

		BookPtr insert_book(Book& book) const;

	private:
		void insert_page(
			const Page& page, 
			sql::PreparedStatement& ps,
			sql::PreparedStatement& ls,
			sql::PreparedStatement& cs
		) const;
		void insert_line(
			const Line& line, 
			sql::PreparedStatement& ls,
			sql::PreparedStatement& cs
		) const;
		void check_session_lock() const;
		int last_insert_id(sql::Connection& conn) const;
		sql::Connection* connection() const;

		static UserPtr get_user_from_result_set(ResultSetPtr res);
		static PreparedStatementPtr make_insert_line_statement(sql::Connection& conn);
		static PreparedStatementPtr make_insert_page_statement(sql::Connection& conn);
		static PreparedStatementPtr make_insert_content_statement(sql::Connection& conn);

		boost::optional<ScopeGuard> scope_guard_;
		const SessionPtr session_;
		const ConfigPtr config_;
	};
}

#endif // pcw_Database_hpp__
