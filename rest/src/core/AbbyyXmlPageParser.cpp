#include <cstring>
#include <pugixml.hpp>
#include "AbbyyXmlPageParser.hpp"
#include "Page.hpp"
#include "Line.hpp"
#include "Box.hpp"
#include "util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlPageParser::AbbyyXmlPageParser(const Path& path)
	: XmlFile(path)
	, page_()
{
	page_ = xml_.select_node("/document/page").node();
}

////////////////////////////////////////////////////////////////////////////////
bool
AbbyyXmlPageParser::has_next() const noexcept
{
	return page_;
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
AbbyyXmlPageParser::next() 
{
	assert(page_);
	auto page = std::make_shared<Page>(0);
	page->ocr = path_;
	page->box = {0, 0, page_.attribute("width").as_int(), page_.attribute("heigth").as_int()};
	auto linenodes = page_.select_nodes(".//line");
	int lineid = 0;
	for (const auto& i: linenodes) {
		Line line(++lineid);
		const auto l = i.node().attribute("l").as_int();
		const auto t = i.node().attribute("t").as_int();
		const auto r = i.node().attribute("r").as_int();
		const auto b = i.node().attribute("b").as_int();
		line.box = {l, t, r, b};
		for (const auto& j: i.node().children("charParams")) {
			const auto r = j.attribute("r").as_int();
			const auto l = j.attribute("l").as_int();
			// char confidence is saved as integer <= 100
			const double c = j.attribute("charConfidence").as_int(); 
			const auto str = j.child_value();
			line.append(str, 0, l, r, c/100.0);	
		}
		page->push_back(std::move(line));
	}
	page_ = page_.next_sibling("page");
	return page;
}

