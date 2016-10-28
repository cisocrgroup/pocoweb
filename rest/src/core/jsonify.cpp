#include <crow/json.h>
#include "Box.hpp"
#include "Page.hpp"
#include "jsonify.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Json&
pcw::operator<<(Json& json, const Project& project)
{
	json["id"] = project.origin().id();
	json["uri"] = project.origin().uri;
	json["author"] = project.origin().author;
	json["title"] = project.origin().title;
	json["year"] = project.origin().year;
	json["description"] = project.origin().description;
	json["isBook"] = project.is_book();
	
	std::vector<int> ids;
	ids.resize(project.size());
	std::transform(begin(project), end(project), begin(ids), [](const auto& page) {
		assert(page);
		return page->id;
	});
	json["pages"] = ids.size();
	json["pageIds"] = ids;
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& 
pcw::operator<<(Json& json, const Page& page)
{
	json["id"] = page.id;
	json["box"] << page.box;
	json["ocrFile"] = page.ocr.native();
	json["imgFile"] = page.img.native();
	
	// add from left to right
	size_t i = 0;
	for (const auto& line: page) {
		json["lines"][i++] << line;
	}
	return json;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Json& 
pcw::operator<<(Json& json, const Line& line)
{
	json["id"] = line.id;
	json["box"] << line.box;
	json["imgFile"] = line.img.native();
	json["string"] = line.string();
	json["cuts"] = line.cuts();
	json["confidences"] = line.confidences();
	size_t i = 0;
	for (auto b: line.corrections()) 
		json["corrections"][i++] = b;
	i = 0;
	line.each_word([&i,&json](const auto& word) {
		json["words"][i]["corrected"] = word.corrected;
		json["words"][i]["word"] = word.word;
		json["words"][i]["confidence"] = word.confidence;
		json["words"][i]["box"] << word.box;
		++i;
	});
	json["averageConfidence"] = line.calculate_average_confidence();
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
