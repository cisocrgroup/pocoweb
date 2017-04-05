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
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;
	class Book;
	class Page;
	class Line;
	class Box;
	class Profile;
	struct Suggestion;
	struct Pattern;

	using Json = crow::json::wvalue;
	Json& operator<<(Json& j, const std::vector<ProjectPtr>& books);
	Json& operator<<(Json& j, const Project& project);
	Json& operator<<(Json& j, const Page& page);
	Json& operator<<(Json& j, const Line& line);
	Json& operator<<(Json& j, const Box& box);
	Json& operator<<(Json& j, const Profile& profile);
	Json& operator<<(Json& j, const Suggestion& sugg);
	Json& operator<<(Json& j, const std::vector<Suggestion>& suggs);
	Json& operator<<(Json& j, const std::map<Pattern, std::vector<Suggestion>>& x);
}

#endif // pcw_jsonify_hpp__
