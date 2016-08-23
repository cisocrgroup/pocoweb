#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

#include <string>

namespace pcw {
	struct Config {
		static Config load(const std::string& filename);

		const struct Db {
			const std::string user, host, pass;
		} db;

		const struct Daemon {
			const std::string host, user;
			const int port, threads, sessions;
			const bool detach;
		} daemon;

		const struct Log {
			const std::string file;
		} log;
	};
}

#endif // pcw_Config_hpp__
