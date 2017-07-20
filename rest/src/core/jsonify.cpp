#include "jsonify.hpp"
#include <crow/json.h>
#include "Box.hpp"
#include "Page.hpp"
#include "Profile.hpp"
#include "User.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const std::vector<ProjectPtr>& books) {
	int i = 0;
	for (const auto& book : books) {
		json["books"][i++] << *book;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const BookData& data) {
	json["uri"] = data.uri;
	json["author"] = data.author;
	json["title"] = data.title;
	json["year"] = data.year;
	json["language"] = data.lang;
	json["description"] = data.description;
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Project& view) {
	json["projectId"] = view.origin().id();
	json["isBook"] = view.is_book();
	json << view.origin().data;

	std::vector<int> ids;
	ids.resize(view.size());
	std::transform(begin(view), end(view), begin(ids),
		       [](const auto& page) {
			       assert(page);
			       return page->id();
		       });
	json["pages"] = ids.size();
	json["pageIds"] = ids;
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Page& page) {
	json["pageId"] = page.id();
	json["projectId"] = page.book().id();
	json["box"] << page.box;
	json["ocrFile"] = page.ocr.native();
	json["imgFile"] = page.img.native();

	// add from left to right
	size_t i = 0;
	for (const auto& line : page) {
		json["lines"][i++] << *line;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Line& line) {
	json["lineId"] = line.id();
	json["pageId"] = line.page().id();
	json["projectId"] = line.page().book().id();
	json["box"] << line.box;
	json["imgFile"] = line.img.native();
	json["cor"] = line.cor();
	json["ocr"] = line.ocr();
	json["cuts"] = line.cuts();
	json["confidences"] = line.confidences();
	json["averageConfidence"] = line.average_conf();
	json["isFullyCorrected"] = line.is_fully_corrected();
	json["isPartiallyCorrected"] = line.is_partially_corrected();
	// do *not* show words of each line
	// /books/id/page/id/lines/id/tokens will give all tokens of a line
	// size_t i = 0;
	// line.each_token([&i, &json](const auto& token) {
	// 	json["tokens"][i] << token;
	// 	++i;
	// });
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Token& token) {
	json["projectId"] = token.line->page().book().id();
	json["pageId"] = token.line->page().id();
	json["lineId"] = token.line->id();
	json["offset"] = token.offset();
	json["tokenId"] = token.id;
	json["isFullyCorrected"] = token.is_fully_corrected();
	json["isPartiallyCorrected"] = token.is_partially_corrected();
	json["ocr"] = token.ocr();
	json["cor"] = token.cor();
	json["averageConf"] = token.average_conf();
	json["box"] << token.box;
	json["isNormal"] = token.is_normal();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Box& box) {
	json["left"] = box.left();
	json["right"] = box.right();
	json["top"] = box.top();
	json["bottom"] = box.bottom();
	json["width"] = box.width();
	json["height"] = box.height();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Suggestion& sugg) {
	json["cor"] = sugg.cand.cor();
	json["ocr"] = sugg.token.cor();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const std::vector<Suggestion>& suggs) {
	size_t i = 0;
	for (const auto& s : suggs) {
		json["suggestions"][i++] << s;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(
    Json& json, const std::map<Pattern, std::vector<Suggestion>>& x) {
	for (const auto& p : x) {
		json[p.first.cor + ":" + p.first.ocr] << p.second;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Profile& profile) {
	json["projectId"] = profile.book().id();
	json["suggestions"] << profile.suggestions();
	json["histPatterns"] << profile.calc_hist_patterns();
	json["ocrPatterns"] << profile.calc_ocr_patterns();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& j, const User& user) {
	j["name"] = user.name;
	j["email"] = user.email;
	j["institute"] = user.institute;
	j["id"] = user.id();
	j["admin"] = user.admin();
	return j;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson& j, const char* key, bool& res) {
	if (j.has(key)) {
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
bool pcw::get(const RJson& j, const char* key, int& res) {
	if (j.has(key) and j[key].t() == crow::json::type::Number) {
		res = j[key].i();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson& j, const char* key, double& res) {
	if (j.has(key) and j[key].t() == crow::json::type::Number) {
		res = j[key].d();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::get(const RJson& j, const char* key, std::string& res) {
	if (j.has(key) and j[key].t() == crow::json::type::String) {
		res = j[key].s();
		return true;
	}
	return false;
}
