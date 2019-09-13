#ifndef pcw_jsonify_hpp__
#define pcw_jsonify_hpp__

#include <boost/optional.hpp>
#include <map>
#include <memory>
#include <vector>

namespace crow {
class query_string;
namespace json {
class wvalue;
class rvalue;
} // namespace json
} // namespace crow

namespace pcw {
class Project;
using ProjectPtr = std::shared_ptr<Project>;
class Book;
struct BookData;
class Page;
class Line;
struct Token;
class Box;

double fix_double(double val);

using Json = crow::json::wvalue;
using RJson = crow::json::rvalue;
Json &operator<<(Json &j, const std::vector<ProjectPtr> &books);
Json &operator<<(Json &j, const BookData &data);
Json &operator<<(Json &j, const Project &project);
Json &operator<<(Json &j, const Page &page);
Json &operator<<(Json &j, const Line &line);
Json &operator<<(Json &j, const Token &token);
Json &operator<<(Json &j, const std::vector<Token> &tokens);
Json &operator<<(Json &j, const Box &box);

Json &wj(Json &j, const std::vector<ProjectPtr> &books);
Json &wj(Json &j, const BookData &data);
Json &wj(Json &j, const Project &project);
Json &wj(Json &j, const Page &page, int projectid);
Json &wj(Json &j, const Line &line, int projectid);
Json &wj(Json &j, const Token &token, int projectid);
Json &wj(Json &j, const std::vector<Token> &tokens, int projectid);
Json &wj(Json &j, const Box &box);

using Query = crow::query_string;
template <class T> boost::optional<T> get(const Query &params, const char *key);

bool get(const Query &q, const char *key, bool &out);
bool get(const Query &q, const char *key, int &out);
bool get(const Query &q, const char *key, double &out);
bool get(const Query &q, const char *key, std::string &out);
bool get(const Query &q, const char *key, std::vector<std::string> &out);

template <class T> boost::optional<T> get(const RJson &j, const char *key);
template <class T, class F> void ifset(const RJson &j, const char *key, F f);
bool get(const RJson &j, const char *key, std::vector<int> &res);
bool get(const RJson &j, const char *key, bool &res);
bool get(const RJson &j, const char *key, int &res);
bool get(const RJson &j, const char *key, double &res);
bool get(const RJson &j, const char *key, std::string &res);
} // namespace pcw

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline boost::optional<T> pcw::get(const crow::query_string &params,
                                   const char *key) {
  T t;
  if (get(params, key, t)) {
    return t;
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline boost::optional<T> pcw::get(const RJson &j, const char *key) {
  T t;
  if (get(j, key, t)) {
    return t;
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
template <class T, class F>
inline void pcw::ifset(const RJson &j, const char *key, F f) {
  T t;
  if (get(j, key, t)) {
    f(t);
  }
}

#endif // pcw_jsonify_hpp__
