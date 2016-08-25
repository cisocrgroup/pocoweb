#ifndef pcw_BookData_hpp__
#define pcw_BookData_hpp__

namespace sql {
	class Connection;
}

namespace pcw {
	struct BookData {
		BookData() = default;
		BookData(nlohmann::json& json): BookData() {load(json);}
		std::string title, author, desc, uri, path;
		int id, year, firstpage, lastpage, owner;
		
		void load(sql::Connection& c);
		void store(sql::Connection& c) const;
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
	};
}

#endif // pcw_BookData_hpp__
