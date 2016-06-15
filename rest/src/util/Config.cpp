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
	db.user = tree.get<std::string>("db.user");
	db.host = tree.get<std::string>("db.host");
	db.pass = tree.get<std::string>("db.pass");
	daemon.host = tree.get<std::string>("daemon.host");
	daemon.port = tree.get<int>("daemon.port");
	daemon.threads = tree.get<int>("daemon.threads");
	log.file = tree.get<std::string>("log.file");
	return *this;
}
