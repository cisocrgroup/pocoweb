#ifndef pcw_Database_hpp__
#define pcw_Database_hpp__

#include <boost/optional.hpp>
#include <memory>
#include "ScopeGuard.hpp"
#include "db.hpp"

namespace pcw {
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;
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
	struct AppCache;
	using CachePtr = std::shared_ptr<AppCache>;
	class Line;

	class Database {
	public:
		Database(SessionPtr session, ConfigPtr config, CachePtr cache = nullptr);
		
		void set_autocommit(bool ac = true);
		bool autocommit() const noexcept;
		void commit();
		Session& session() const noexcept {return *session_;}

		UserPtr insert_user(const std::string& name, const std::string& pass) const;
		UserPtr authenticate(const std::string& name, const std::string& pass) const;
		UserPtr select_user(const std::string& name) const;
		UserPtr select_user(int userid) const;
		void update_user(const User& user) const;
		void delete_user(const std::string& name) const;

		BookPtr insert_book(Book& book) const;
		ProjectPtr insert_project(Project& project) const;
		ProjectPtr select_project(int projectid) const;
		std::vector<ProjectPtr> select_all_projects(const User& user) const;

	private:
		UserPtr select_user(const std::string& name, sql::Connection& conn) const;
		UserPtr select_user(int userid, sql::Connection& conn) const;
		ProjectPtr select_project(int projectid, sql::Connection& conn) const;
		ProjectPtr select_subproject(int projectid, int origin, int owner, sql::Connection& conn) const;
		ProjectPtr select_subproject(int projectid, int owner, const Book& book, sql::Connection& conn) const;
		BookPtr select_book(int bookid, int owner, sql::Connection& conn) const;
		int insert_book_project(const Project& project, sql::Connection& conn) const;
		void update_project_origin_id(int id, sql::Connection& conn) const;
		void insert_page(const Page& page, sql::Connection& conn) const;
		void insert_line(const Line& line, sql::Connection& conn) const; 
		void select_all_pages(Book& book, sql::Connection& conn) const;
		void select_all_lines(Page& page, sql::Connection& conn) const;
		void check_session_lock() const;
		int last_insert_id(sql::Connection& conn) const;
		sql::Connection* connection() const;

		UserPtr cached_select_user(const std::string& name, sql::Connection& conn) const;
		UserPtr cached_select_user(int userid, sql::Connection& conn) const;
		ProjectPtr cached_select_project(int prid, sql::Connection& conn) const;
		UserPtr put_cache(UserPtr user) const;
		ProjectPtr put_cache(ProjectPtr proj) const;

		static UserPtr get_user_from_result_set(ResultSetPtr res);

		boost::optional<ScopeGuard> scope_guard_;
		const SessionPtr session_;
		const ConfigPtr config_;
		const CachePtr cache_;
	};
}

#endif // pcw_Database_hpp__
