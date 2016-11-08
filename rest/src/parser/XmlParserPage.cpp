#include <pugixml.hpp>
#include "XmlParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
XmlParserPage::XmlParserPage(const Path& path)
	: lines_()
	, doc_(std::make_shared<pugi::xml_document>())
{
	assert(doc_);
	auto ok = doc_->load_file(path.string().data());
	if (not ok) 
		throw std::runtime_error(
			"(XmlParserPage) Could not read	" + 
			path.string() +
			ok.description()
		);
	this->ocr = path;
}

////////////////////////////////////////////////////////////////////////////////
void 
XmlParserPage::write(const Path& path) const 
{
	assert(doc_);
	doc_->save_file(path.string().data());
}
