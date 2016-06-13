#ifndef pcw_db_hpp__
#define pcw_db_hpp__

namespace sql {
	class Connection;
	class Statement;
	class PreparedStatement;
	class ResultSet;
}

namespace pcw {
	using DriverPtr = std::shared_ptr<sql::Driver>;
	using ConnectionPtr = std::shared_ptr<sql::Connection>;
	using StatementPtr = std::unique_ptr<sql::Statement>;
	using PreparedStatementPtr = std::unique_ptr<sql::PreparedStatement>;
	using ResultSetPtr = std::unique_ptr<sql::ResultSet>;
}

#endif // pcw_db_hpp__
