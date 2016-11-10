#include <stdexcept>
#include <pugixml.hpp>
#include "XmlParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
XmlParserPage::XmlParserPage(DocPtr doc)
	: lines_()
	, doc_(std::move(doc))
{
}

////////////////////////////////////////////////////////////////////////////////
XmlParserPage::XmlParserPage(const Path& path)
	: lines_()
	, doc_()
{
	read(path);
}

////////////////////////////////////////////////////////////////////////////////
void
XmlParserPage::read(const Path& path)
{
	doc_ = std::make_shared<Doc>();
	auto ok = doc_->load_file(path.string().data());
	if (not ok)
		throw std::runtime_error(
			"(XmlParserPage) Could not read	" +
			path.string() + ": " +
			ok.description()
		);
	lines_.clear();
	this->ocr = path;
}

////////////////////////////////////////////////////////////////////////////////
void
XmlParserPage::write(const Path& path) const
{
	assert(doc_);
	doc_->save_file(path.string().data());
}
