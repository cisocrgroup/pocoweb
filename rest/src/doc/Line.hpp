#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include <boost/filesystem/path.hpp>
#include "Box.hpp"

namespace sql {
	class Connection;
}

namespace pcw {
	using Path = boost::filesystem::path;

	class Line: public std::enable_shared_from_this<Line> {
	public:
		using Cuts = std::vector<int>;

		Line(int id);
		Line(nlohmann::json& json) {load(json);}
		virtual ~Line() noexcept = default;
		const std::string& line() const noexcept {return line_;}
		std::string& line() noexcept {return line_;}
		const Cuts& cuts() const noexcept {return cuts_;}
		Cuts& cuts() noexcept {return cuts_;}
		std::string cuts_str() const noexcept;
		void dbload(sql::Connection& c, int bookid, int pageid);
		void dbstore(sql::Connection& c, int bookid, int pageid) const;
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;

		Box box;
		Path imagefile;
		int id;

	private:
		static Cuts cuts_from_str(const std::string& str); 
		std::string line_;
		Cuts cuts_;
	};
	using LinePtr = std::shared_ptr<Line>;
}

#endif // pcw_Line_hpp__

