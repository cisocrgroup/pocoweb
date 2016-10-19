// #include <boost/log/trivial.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <regex>
#include <thread>
#include "Config.hpp"

////////////////////////////////////////////////////////////////////////////////
static std::unordered_map<std::string, pcw::Ptree>
get_plugins(const pcw::Ptree& ptree)
{
	static const std::regex plugin{R"(^plugin[-_](.*)$)", std::regex_constants::icase};
	std::unordered_map<std::string, pcw::Ptree> plugins;
	std::smatch m;
	for (const auto& config: ptree) {
		if (std::regex_search(config.first, m, plugin)) {
			plugins.emplace(m[1], config.second);
		}
	}
	return plugins;
}

// ////////////////////////////////////////////////////////////////////////////////
// static int
// get_log_level(const std::string& level)
// {
// 	if (level == "trace")
// 		return boost::log::trivial::trace;
// 	if (level == "debug")
// 		return boost::log::trivial::debug;
// 	if (level == "info")
// 		return boost::log::trivial::info;
// 	if (level == "warning")
// 		return boost::log::trivial::warning;
// 	if (level == "error")
// 		return boost::log::trivial::error;
// 	if (level == "fatal")
// 		return boost::log::trivial::fatal;
// 	BOOST_LOG_TRIVIAL(warning) << "Invalid log level `"
// 				   << level << "` defaulting to `info`";
// 	return boost::log::trivial::info;
// }

////////////////////////////////////////////////////////////////////////////////
pcw::Config
pcw::Config::load(const std::string& filename)
{
	Ptree ptree;
	boost::property_tree::read_ini(filename, ptree);
	const auto detach = ptree.get<bool>("daemon.detach");
	const std::string logfile = detach ?
		ptree.get<std::string>("log.file") :
		"/dev/stderr";
	auto threads = ptree.get<int>("daemon.threads");
	threads = threads < 1 ? std::thread::hardware_concurrency() : threads;

	return {
			{
				ptree.get<std::string>("db.user"),
				ptree.get<std::string>("db.host"),
				ptree.get<std::string>("db.pass"),
				ptree.get<std::string>("db.db")
			},
			{
				ptree.get<std::string>("daemon.host"),
				ptree.get<std::string>("daemon.user"),
				ptree.get<std::string>("daemon.group"),
				ptree.get<std::string>("daemon.basedir"),
				ptree.get<int>("daemon.port"),
				threads,
				ptree.get<int>("daemon.sessions"),
				detach
			},
			{
				logfile,
				// get_log_level(ptree.get<std::string>("log.level"))
			},
			{
				get_plugins(ptree),
			},
	};

}

////////////////////////////////////////////////////////////////////////////////
const pcw::Ptree&
pcw::Config::Plugins::operator[](const std::string& p) const noexcept
{
	static const Ptree nothing;
	auto i = configs.find(p);
	return i == end(configs) ? nothing : i->second;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
pcw::operator<<(std::ostream& os, const Config& config)
{
	os << "db.user: " << config.db.user << "\n";
	os << "db.host: " << config.db.host << "\n";
	os << "db.pass: " << config.db.pass << "\n";
	os << "db.db: " << config.db.db << "\n";

	os << "daemon.host: " << config.daemon.host << "\n";
	os << "daemon.user: " << config.daemon.user << "\n";
	os << "daemon.group: " << config.daemon.group << "\n";
	os << "daemon.basedir: " << config.daemon.basedir << "\n";
	os << "daemon.port: " << config.daemon.port << "\n";
	os << "daemon.threads: " << config.daemon.threads << "\n";
	os << "daemon.sessions: " << config.daemon.sessions << "\n";
	os << "daemon.detach: " << config.daemon.detach << "\n";

	os << "log.file: " << config.log.file << "\n";
	os << "log.level: " << config.log.level;

	for (const auto& p: config.plugins.configs) {
		for (const auto& q: p.second) {
			os << "\nplugins." << p.first << "." << q.first << ": ";
			os << q.second.data();
		}
	}
	return os;
}
