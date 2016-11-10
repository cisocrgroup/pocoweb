#include <cstring>
#include <pugixml.hpp>
#include "AbbyyXmlPageParser.hpp"
#include "core/Page.hpp"
#include "core/Line.hpp"
#include "core/Box.hpp"
#include "core/util.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlPageParser::AbbyyXmlPageParser(Xml xml)
	: path_()
	, xml_(xml)
	, page_()
{
	page_ = xml_.doc().select_node("/document/page").node();
}

////////////////////////////////////////////////////////////////////////////////
AbbyyXmlPageParser::AbbyyXmlPageParser(const Path& path)
	: path_(path)
	, xml_(path)
	, page_()
{
	page_ = xml_.doc().select_node("/document/page").node();
}

////////////////////////////////////////////////////////////////////////////////
bool
AbbyyXmlPageParser::has_next() const noexcept
{
	return page_;
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
AbbyyXmlPageParser::parse()
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
			line.append(str, 0, l, r, c/100.0, false);
		}
		page->push_back(std::move(line));
	}
	page_ = page_.next_sibling("page");
	return page;
}

