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
	json["id"] = view.origin().id();
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
	json["id"] = page.id();
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
	json["id"] = line.id();
	json["box"] << line.box;
	json["imgFile"] = line.img.native();
	json["cor"] = line.cor();
	json["ocr"] = line.ocr();
	json["cuts"] = line.cuts();
	json["confidences"] = line.confidences();
	json["averageConfidence"] = line.average_conf();
	json["isCorrected"] = line.is_corrected();
	size_t i = 0;
	line.each_token([&i, &json](const auto& token) {
		json["tokens"][i] << token;
		++i;
	});
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& pcw::operator<<(Json& json, const Token& token) {
	json = token.is_corrected();
	json = token.ocr();
	json = token.cor();
	json = token.average_conf();
	json << token.box;
	json = token.is_normal();
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
	json["book"] = profile.book().id();
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
