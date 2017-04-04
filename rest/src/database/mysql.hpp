#ifndef pcw_mysql_hpp__
#define pcw_mysql_hpp__

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "core/Config.hpp"
#include "utils/ScopeGuard.hpp"
#include "database/ConnectionPool.hpp"

namespace pcw {
	using MysqlConnectionConfigSptr =
		std::shared_ptr<sqlpp::mysql::connection_config>;
	using MysqlConnection = Connection<sqlpp::mysql::connection>;
	using MysqlConnectionPool =
		ConnectionPool<sqlpp::mysql::connection, MysqlConnectionConfigSptr>;
	using MysqlConnectionPoolSptr = std::shared_ptr<MysqlConnectionPool>;

	class MysqlCommiter: public ScopeGuard {
	public:
		MysqlCommiter(const MysqlConnection& connection)
			: ScopeGuard([this](){this->rollback();})
			, connection_(connection)
		{
			connection_.db().start_transaction();
		}
		void commit()
		{
			connection_.db().commit_transaction();
			dismiss();
		}
		void rollback()
		{
			connection_.db().rollback_transaction(false);
		}

	private:
		const MysqlConnection& connection_;
	};

	inline MysqlConnectionConfigSptr
	mysqlConnectionConfigFromConfig(const Config& config)
	{
		auto c = std::make_shared<sqlpp::mysql::connection_config>();
		c->user = config.db.user;
		c->password = config.db.pass;
		c->host = config.db.host;
		c->database = config.db.db;
		c->debug = config.db.debug;
		return c;
	}
}

#endif // pcw_mysql_hpp__
