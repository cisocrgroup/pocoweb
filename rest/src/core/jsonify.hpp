#ifndef pcw_jsonify_hpp__
#define pcw_jsonify_hpp__

#include <map>
#include <memory>
#include <vector>

namespace crow {
	namespace json {
		class wvalue;
	}
}

namespace pcw {
	class BookView;
	using BookViewPtr = std::shared_ptr<BookView>;
	class Book;
	class Page;
	class Line;
	class Box;
	class Profile;
	class Suggestion;
	class Pattern;

	using Json = crow::json::wvalue;
	Json& operator<<(Json& j, const std::vector<BookViewPtr>& books);
	Json& operator<<(Json& j, const BookView& project);
	Json& operator<<(Json& j, const Page& page);
	Json& operator<<(Json& j, const Line& line);
	Json& operator<<(Json& j, const Box& box);
	Json& operator<<(Json& j, const Profile& profile);
	Json& operator<<(Json& j, const Suggestion& sugg);
	Json& operator<<(Json& j, const std::vector<Suggestion>& suggs);
	Json& operator<<(Json& j, const std::map<Pattern, std::vector<Suggestion>>& x);
}

#endif // pcw_jsonify_hpp__
