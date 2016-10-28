#ifndef pcw_jsonify_hpp__
#define pcw_jsonify_hpp__

namespace crow {
	namespace json {
		class wvalue;
	}
}

namespace pcw {
	class Project;
	class Book;
	class Page;
	class Line;
	class Box;

	using Json = crow::json::wvalue;
	Json& operator<<(Json& j, const Project& project);
	Json& operator<<(Json& j, const Page& page);
	Json& operator<<(Json& j, const Line& line);
	Json& operator<<(Json& j, const Box& box);
}

#endif // pcw_jsonify_hpp__
