#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Config.hpp"

namespace pt = boost::property_tree;

////////////////////////////////////////////////////////////////////////////////
pcw::Config&
pcw::Config::load(const std::string& filename)
{
	pt::ptree tree;
	pt::read_ini(filename, tree);
	dbUser = tree.get<std::string>("db.user");
	dbHost = tree.get<std::string>("db.host");
	dbPass = tree.get<std::string>("db.pass");
	daemonHost = tree.get<std::string>("daemon.host");
	daemonPort = tree.get<int>("daemon.port");
	daemonThreads = tree.get<int>("daemon.threads");
	return *this;
}
