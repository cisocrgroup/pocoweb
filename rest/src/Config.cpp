#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Config.hpp"

namespace pt = boost::property_tree;

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
				tree.get<std::string>("db.pass")
			},
			{
				tree.get<std::string>("daemon.host"),
				tree.get<std::string>("daemon.user"),
				tree.get<int>("daemon.port"),
				tree.get<int>("daemon.threads"),
				tree.get<int>("daemon.sessions"),
				detach
			},
			{
				tree.get<size_t>("sessions.n")
			},
			{
				logfile
			}
	};
}
