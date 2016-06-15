#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

namespace pcw {
	struct Config {
		Config& load(const std::string& filename);

		struct Db {
			std::string user, host, pass;
		} db;
		struct Daemon {
			std::string host;
			int port, threads;
		} daemon;
		struct Log {
			std::string file;
		} log;
	};
}

#endif // pcw_Config_hpp__
