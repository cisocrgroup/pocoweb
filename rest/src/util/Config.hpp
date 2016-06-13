#ifndef pcw_Config_hpp__
#define pcw_Config_hpp__

namespace pcw {
	struct Config {
		Config& load(const std::string& filename);

		std::string dbUser, dbHost, dbPass;
	};
}

#endif // pcw_Config_hpp__
