#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_map>

namespace pcw {
	using Ptree = boost::property_tree::ptree;

	struct Config {
		static Config load(const std::string& filename);

		const struct Db {
			const std::string user, host, pass, db;
		} db;

		const struct Daemon {
			const std::string host, user, group, basedir;
			const int port, threads, sessions;
			const bool detach;
		} daemon;

		const struct Log {
			const std::string file;
			const int level;
		} log;

		const struct Plugins {
			const std::unordered_map<std::string, Ptree> configs;
			const Ptree& operator[](const std::string& p) const noexcept;
		} plugins;
		void LOG() const;
	};
}

#endif // pcw_Config_hpp__
