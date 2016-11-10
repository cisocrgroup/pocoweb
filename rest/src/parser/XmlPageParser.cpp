#include <pugixml.hpp>
#include "XmlPageParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
XmlPageParser::XmlPageParser(DocPtr doc)
	: doc_(std::move(doc))
{}

////////////////////////////////////////////////////////////////////////////////
XmlPageParser::XmlPageParser(const Path& path)
	: doc_()
{
	read(path);
}

////////////////////////////////////////////////////////////////////////////////
void
XmlPageParser::read(const Path& path)
{
	doc_ = std::make_shared<pugi::xml_document>();
	auto ok = doc_->load_file(path.string().data());
	if (not ok)
		throw std::runtime_error(
				"(XmlPageParser) Could not read file " +
				path.string() + ": " +
				ok.description()
		);
}
