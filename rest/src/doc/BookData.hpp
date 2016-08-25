#ifndef pcw_BookData_hpp__
#define pcw_BookData_hpp__

#include <boost/filesystem/path.hpp>

namespace sql {
	class Connection;
}

namespace pcw {
	using Path = boost::filesystem::path;

	struct BookData {
		BookData() = default;
		BookData(nlohmann::json& json): BookData() {load(json);}
		std::string title, author, desc, uri;
		Path path;
		int id, year, firstpage, lastpage, owner;
		
		void dbstore(sql::Connection& c) const;
		void dbload(sql::Connection& c);
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
	};
}

#endif // pcw_BookData_hpp__
