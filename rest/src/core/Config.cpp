// #include <boost/log/trivial.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <crow/logging.h>
#include <boost/property_tree/ini_parser.hpp>
#include <regex>
#include <thread>
#include "Logger.hpp"
#include "Error.hpp"
#include "Config.hpp"

#ifndef PCW_CONFIG_EXPANSION_MAX_RUNS
#	define PCW_CONFIG_EXPANSION_MAX_RUNS 1000
#endif // PCW_CONFIG_EXPANSION_MAX_RUNS

using PluginsConfig = std::unordered_map<std::string, pcw::Ptree>;

////////////////////////////////////////////////////////////////////////////////
static std::string
get_val(const pcw::Ptree& ptree, const std::string& var)
{
	auto val = getenv(var.data());
	if (val)
		return val;
	else
		return ptree.get<std::string>(var);
}

////////////////////////////////////////////////////////////////////////////////
static void
expand_variables(pcw::Ptree& ptree)
{
	static const std::regex var{R"(\$[({](.+)[})])"};
	std::smatch m;
	size_t runs = 0;
again:
	if (runs++ >= PCW_CONFIG_EXPANSION_MAX_RUNS)
		THROW(pcw::Error, "Maximal number of expansion runs exeeded (",
				"Recursion in a variable?): ", runs);
	for (auto& p: ptree) {
		for (auto& pair: p.second) {
			if (std::regex_search(pair.second.data(), m, var)) {
				auto expand = get_val(ptree, m[1]);
				auto res = std::regex_replace(pair.second.data(),
						var, expand, std::regex_constants::format_first_only);
				p.second.put(pair.first, res);
				goto again;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
static PluginsConfig
get_plugins(const pcw::Ptree& ptree)
{
	static const std::regex plugin{R"(^plugin[-_](.*)$)", std::regex_constants::icase};
	PluginsConfig plugins;
	std::smatch m;
	for (const auto& config: ptree) {
		if (std::regex_search(config.first, m, plugin)) {
			plugins.emplace(m[1], config.second);
		}
	}
	return plugins;
}

////////////////////////////////////////////////////////////////////////////////
static int
get_log_level(const std::string& level)
{
	if (boost::iequals(level, "debug"))
		return static_cast<int>(crow::LogLevel::Debug);
	if (boost::iequals(level, "info"))
		return static_cast<int>(crow::LogLevel::Info);
	if (boost::iequals(level, "warning"))
		return static_cast<int>(crow::LogLevel::Warning);
	if (boost::iequals(level, "error"))
		return static_cast<int>(crow::LogLevel::Error);
	if (boost::iequals(level, "critical"))
		return static_cast<int>(crow::LogLevel::Critical);
	CROW_LOG_WARNING << "(Config) Invalid log level: " << level << "; "
			 << "defaulting to info";
	return static_cast<int>(crow::LogLevel::Info);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Config
pcw::Config::load(const std::string& filename)
{
	Ptree ptree;
	boost::property_tree::read_ini(filename, ptree);
	expand_variables(ptree);
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
				get_log_level(ptree.get<std::string>("log.level"))
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
void
pcw::Config::setup_logging() const
{
	static crow::CerrLogHandler cerrlogger;
	static Syslogger syslogger;
	if (daemon.detach)
		crow::logger::setHandler(&syslogger);
	else
		crow::logger::setHandler(&cerrlogger);
	crow::logger::setLogLevel(static_cast<crow::LogLevel>(log.level));
}

////////////////////////////////////////////////////////////////////////////////
void
pcw::Config::LOG() const
{
	CROW_LOG_INFO << "db.user: " << this->db.user;
	CROW_LOG_INFO << "db.host: " << this->db.host;
	CROW_LOG_INFO << "db.pass: " << this->db.pass;
	CROW_LOG_INFO << "db.db: " << this->db.db;

	CROW_LOG_INFO << "daemon.host: " << this->daemon.host;
	CROW_LOG_INFO << "daemon.user: " << this->daemon.user;
	CROW_LOG_INFO << "daemon.group: " << this->daemon.group;
	CROW_LOG_INFO << "daemon.basedir: " << this->daemon.basedir;
	CROW_LOG_INFO << "daemon.port: " << this->daemon.port;
	CROW_LOG_INFO << "daemon.threads: " << this->daemon.threads;
	CROW_LOG_INFO << "daemon.sessions: " << this->daemon.sessions;
	CROW_LOG_INFO << "daemon.detach: " << this->daemon.detach;

	CROW_LOG_INFO << "log.file: " << this->log.file;
	CROW_LOG_INFO << "log.level: " << this->log.level;

	for (const auto& p: this->plugins.configs) {
		for (const auto& q: p.second) {
			CROW_LOG_INFO << "plugins." << p.first << "."
				<< q.first << ": " << q.second.data();
		}
	}
}
