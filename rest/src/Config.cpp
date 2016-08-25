#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Config.hpp"

namespace pt = boost::property_tree;

////////////////////////////////////////////////////////////////////////////////
static int
get_log_level(const std::string& level)
{
	if (level == "trace")
		return boost::log::trivial::trace;
	if (level == "debug")
		return boost::log::trivial::debug;
	if (level == "info")
		return boost::log::trivial::info;
	if (level == "warning")
		return boost::log::trivial::warning;
	if (level == "error")
		return boost::log::trivial::error;
	if (level == "fatal")
		return boost::log::trivial::fatal;
	BOOST_LOG_TRIVIAL(warning) << "Invalid log level `" 
				   << level << "` defaulting to `info`";
	return boost::log::trivial::info;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Config
pcw::Config::load(const std::string& filename)
{
	pt::ptree tree;
	pt::read_ini(filename, tree);
	const auto detach = tree.get<bool>("daemon.detach");
	const std::string logfile = detach ?
		tree.get<std::string>("log.file") :
		"/dev/stderr";
	
	return {
			{
				tree.get<std::string>("db.user"),
				tree.get<std::string>("db.host"),
				tree.get<std::string>("db.pass"),
				tree.get<std::string>("db.db")
			},
			{
				tree.get<std::string>("daemon.host"),
				tree.get<std::string>("daemon.user"),
				tree.get<std::string>("daemon.group"),
				tree.get<std::string>("daemon.basedir"),
				tree.get<int>("daemon.port"),
				tree.get<int>("daemon.threads"),
				tree.get<int>("daemon.sessions"),
				detach
			},
			{
				logfile,
				get_log_level(tree.get<std::string>("log.level"))
			}
	};
}
