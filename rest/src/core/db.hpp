#ifndef pcw_db_hpp__
#define pcw_db_hpp__

#include <memory>

namespace sql {
	class Connection;
	class Statement;
	class PreparedStatement;
	class ResultSet;
}

namespace pcw {
	using ConnectionPtr = std::unique_ptr<sql::Connection>;
	using StatementPtr = std::unique_ptr<sql::Statement>;
	using PreparedStatementPtr = std::unique_ptr<sql::PreparedStatement>;
	using ResultSetPtr = std::unique_ptr<sql::ResultSet>;

	class Config;
	ConnectionPtr connect(const Config& config);
}

#endif // pcw_db_hpp__
