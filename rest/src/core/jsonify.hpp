#ifndef pcw_jsonify_hpp__
#define pcw_jsonify_hpp__

#include <boost/optional.hpp>
#include <map>
#include <memory>
#include <vector>

namespace crow {
namespace json {
class wvalue;
class rvalue;
}
}

namespace pcw {
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class Book;
struct BookData;
class Page;
class Line;
struct Token;
class Box;
class User;

using Json = crow::json::wvalue;
using RJson = crow::json::rvalue;
Json& operator<<(Json& j, const std::vector<ProjectPtr>& books);
Json& operator<<(Json& j, const BookData& data);
Json& operator<<(Json& j, const Project& project);
Json& operator<<(Json& j, const Page& page);
Json& operator<<(Json& j, const Line& line);
Json& operator<<(Json& j, const Token& token);
Json& operator<<(Json& j, const Box& box);
Json& operator<<(Json& j, const User& user);

template <class T>
boost::optional<T> get(const RJson& j, const char* key) noexcept;
template <class T, class F>
void ifset(const RJson& j, const char* key, F f) noexcept;
bool get(const RJson& j, const char* key, bool& res) noexcept;
bool get(const RJson& j, const char* key, int& res) noexcept;
bool get(const RJson& j, const char* key, double& res) noexcept;
bool get(const RJson& j, const char* key, std::string& res) noexcept;
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline boost::optional<T> pcw::get(const RJson& j, const char* key) noexcept {
	T t;
	if (get(j, key, t)) {
		return t;
	}
	return {};
}

////////////////////////////////////////////////////////////////////////////////
template <class T, class F>
inline void pcw::ifset(const RJson& j, const char* key, F f) noexcept {
	T t;
	if (get(j, key, t)) {
		f(t);
	}
}

#endif  // pcw_jsonify_hpp__
