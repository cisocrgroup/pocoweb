#include <cassert>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "server_http.hpp"
#include "util/Config.hpp"
#include "util/hash.hpp"
#include "api/api.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableUsers.hpp"

using namespace pcw;
static void run(int argc, char **argv);
static Config loadConfig(int argc, char **argv);
static void initLogging(const Config& config);

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char** argv)
{
	try {
		run(argc, argv);
		return EXIT_SUCCESS;
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	}
	return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
void
run(int argc, char** argv)
{
	const auto config = loadConfig(argc, argv);
	initLogging(config);
	if (daemon(0, 0) == -1)
	 	throw std::system_error(errno, std::system_category(), "daemon");

	auto server = Api::server(config);
	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] starting ";
	std::thread sthread([&server](){server->start();});
	sthread.join();
	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] stopping";
}

////////////////////////////////////////////////////////////////////////////////
Config
loadConfig(int argc, char **argv)
{
	if (argc < 2)
		throw std::runtime_error(std::string("Usage: ") +
					 argv[0] +
					 " config-file");
	Config config;
	config.load(argv[1]);
	return config;
}

////////////////////////////////////////////////////////////////////////////////
void
initLogging(const Config& config)
{
	boost::log::add_common_attributes();
	auto fmtTimeStamp = boost::log::expressions::
		format_date_time<boost::posix_time::ptime
				 >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
	auto fmtThreadId = boost::log::expressions::
		attr<boost::log::attributes::current_thread_id::value_type
		     >("ThreadID");
	auto fmtSeverity = boost::log::expressions::
		attr<boost::log::trivial::severity_level
		     >("Severity");
	boost::log::formatter logFmt =
		boost::log::expressions::format("[%1%] [%2%] [%3%] %4%")
		% fmtTimeStamp
		% fmtThreadId
		% fmtSeverity
		% boost::log::expressions::smessage;
	auto fsink = boost::log::add_file_log(
	         boost::log::keywords::file_name = config.log.file,
		 boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		 boost::log::keywords::auto_flush = true,
		 boost::log::keywords::open_mode = std::ios_base::app);
	fsink->set_formatter(logFmt);
	boost::log::core::get()->set_filter(
	         boost::log::trivial::severity >= boost::log::trivial::info
	);
}
