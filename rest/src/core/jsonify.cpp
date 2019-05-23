#include "jsonify.hpp"
#include "Box.hpp"
#include "Page.hpp"
#include <crow/json.h>
#include <limits>

static double fix_double(double val);

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const std::vector<ProjectPtr> &books) {
  int i = 0;
  for (const auto &book : books) {
    json["books"][i++] << *book;
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const BookData &data) {
  json["uri"] = data.uri;
  json["author"] = data.author;
  json["title"] = data.title;
  json["year"] = data.year;
  json["language"] = data.lang;
  json["status"] = data.status;
  json["description"] = data.description;
  json["profilerUrl"] = data.profilerUrl;
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const Project &view) {
  json["projectId"] = view.id();
  json["bookId"] = view.origin().id();
  json["isBook"] = view.is_book();
  json << view.origin().data;

  std::vector<int> ids;
  ids.resize(view.size());
  std::transform(begin(view), end(view), begin(ids), [](const auto &page) {
    assert(page);
    return page->id();
  });
  json["pages"] = ids.size();
  json["pageIds"] = ids;
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const Page &page) {
  json["pageId"] = page.id();
  json["projectId"] = page.book().id();
  json["box"] << page.box;
  json["ocrFile"] = page.ocr.native();
  json["imgFile"] = page.img.native();

  // add from left to right
  size_t i = 0;
  for (const auto &line : page) {
    if (line) {
      json["lines"][i++] << *line;
    }
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const Line &line) {
  json["lineId"] = line.id();
  json["pageId"] = line.page().id();
  json["projectId"] = line.page().book().id();
  json["box"] << line.box;
  json["imgFile"] = line.img.native();
  json["cor"] = line.cor();
  json["ocr"] = line.ocr();
  json["cuts"] = line.cuts();
  json["confidences"] = line.confidences();
  json["averageConfidence"] = fix_double(line.average_conf());
  json["isFullyCorrected"] = line.is_fully_corrected();
  json["isPartiallyCorrected"] = line.is_partially_corrected();
  // do *not* show words of each line
  // /books/id/page/id/lines/id/tokens will give all tokens of a line
  size_t i = 0;
  line.each_token([&i, &json](const auto &token) {
    json["tokens"][i] << token;
    ++i;
  });
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const std::vector<Token> &tokens) {
  size_t i = 0;
  json["tokens"] = crow::json::rvalue(crow::json::type::List);
  for (const auto &token : tokens) {
    json["tokens"][i++] << token;
  }
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const Token &token) {
  json["projectId"] = token.line->page().book().id();
  json["pageId"] = token.line->page().id();
  json["lineId"] = token.line->id();
  json["offset"] = token.offset();
  json["tokenId"] = token.id;
  json["isFullyCorrected"] = token.is_fully_corrected();
  json["isPartiallyCorrected"] = token.is_partially_corrected();
  json["ocr"] = token.ocr();
  json["cor"] = token.cor();
  json["averageConfidence"] = fix_double(token.average_conf());
  json["box"] << token.box;
  json["isNormal"] = token.is_normal();
  return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json &pcw::operator<<(Json &json, const Box &box) {
  json["left"] = box.left();
  json["right"] = box.right();
  json["top"] = box.top();
  json["bottom"] = box.bottom();
  json["width"] = box.width();
  json["height"] = box.height();
  return json;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson &j, const char *key, bool &res) {
  if (j.t() == crow::json::type::Object and j.has(key)) {
    switch (j[key].t()) {
    case crow::json::type::True:
      res = true;
      return true;
    case crow::json::type::False:
      res = false;
      return true;
    default:
      return false;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson &j, const char *key, int &res) {
  if (j.t() == crow::json::type::Object and j.has(key) and
      j[key].t() == crow::json::type::Number) {
    res = j[key].i();
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson &j, const char *key, double &res) {
  if (j.t() == crow::json::type::Object and j.has(key) and
      j[key].t() == crow::json::type::Number) {
    res = j[key].d();
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson &j, const char *key, std::string &res) {
  if (j.t() == crow::json::type::Object and j.has(key) and
      j[key].t() == crow::json::type::String) {
    res = j[key].s();
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
double fix_double(double val) {
  if (std::isnan(val)) {
    return 0;
  }
  if (std::isinf(val) and val > 0) {
    return std::numeric_limits<double>::max();
    return DBL_MAX;
  }
  if (std::isinf(val) and val < 0) {
    return std::numeric_limits<double>::min();
  }
  return val;
}
