#include <crow/json.h>
#include "Box.hpp"
#include "Page.hpp"
#include "Profile.hpp"
#include "jsonify.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const std::vector<BookViewPtr>& books)
{
	int i = 0;
	for (const auto& book: books) {
		json["books"][i++] << *book;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const BookView& view)
{
	json["id"] = view.origin().id();
	json["uri"] = view.origin().uri;
	json["author"] = view.origin().author;
	json["title"] = view.origin().title;
	json["year"] = view.origin().year;
	json["description"] = view.origin().description;
	json["isBook"] = view.is_book();

	std::vector<int> ids;
	ids.resize(view.size());
	std::transform(begin(view), end(view), begin(ids), [](const auto& page) {
		assert(page);
		return page->id();
	});
	json["pages"] = ids.size();
	json["pageIds"] = ids;
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Page& page)
{
	json["id"] = page.id();
	json["box"] << page.box;
	json["ocrFile"] = page.ocr.native();
	json["imgFile"] = page.img.native();

	// add from left to right
	size_t i = 0;
	for (const auto& line: page) {
		json["lines"][i++] << *line;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Line& line)
{
	json["id"] = line.id();
	json["box"] << line.box;
	json["imgFile"] = line.img.native();
	json["cor"] = line.cor();
	json["ocr"] = line.ocr();
	json["cuts"] = line.cuts();
	json["confidences"] = line.confidences();
	json["averageConfidence"] = line.average_conf();
	size_t i = 0;
	line.each_token([&i,&json](const auto& token) {
		json["tokens"][i]["isCorrected"] = token.is_corrected();
		json["tokens"][i]["ocr"] = token.ocr();
		json["tokens"][i]["cor"] = token.cor();
		json["tokens"][i]["confidence"] = token.average_conf();
		json["tokens"][i]["box"] << token.box;
		json["tokens"][i]["isNormal"] = token.is_normal();
		++i;
	});
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Box& box)
{
	json["left"] = box.left();
	json["right"] = box.right();
	json["top"] = box.top();
	json["bottom"] = box.bottom();
	json["width"] = box.width();
	json["height"] = box.height();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Suggestion& sugg)
{
	json["cor"] = sugg.cand.cor();
	json["ocr"] = sugg.token.cor();
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const std::vector<Suggestion>& suggs)
{
	size_t i = 0;
	for (const auto& s: suggs) {
		json["suggestions"][i++] << s;

	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const std::map<Pattern, std::vector<Suggestion>>& x)
{
	for (const auto& p: x) {
		json[p.first.cor + ":" + p.first.ocr] << p.second;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Profile& profile)
{
	json["book"] = profile.book().id();
	json["suggestions"] << profile.suggestions();
	json["histPatterns"] << profile.calc_hist_patterns();
	json["ocrPatterns"] << profile.calc_ocr_patterns();
	return json;
}

