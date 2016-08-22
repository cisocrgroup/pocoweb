#include <memory>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include "Config.hpp"
#include "db.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::ConnectionPtr
pcw::connect(const Config& config)
{
	const auto driver = sql::mysql::get_mysql_driver_instance();
	ConnectionPtr conn{driver->connect(config.db.host,
				     	   config.db.user,
					   config.db.pass)};
	assert(conn);
	conn->setSchema("pocoweb");
	return conn;
}
