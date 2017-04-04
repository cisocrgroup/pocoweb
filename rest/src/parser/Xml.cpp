#include "pugixml.hpp"
#include "utils/Error.hpp"
#include "Xml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Xml::Xml(DocPtr doc)
	: doc_(std::move(doc))
{}

////////////////////////////////////////////////////////////////////////////////
Xml::Xml(const Path& path)
	: doc_()
{
	read(path);
}

////////////////////////////////////////////////////////////////////////////////
void
Xml::read(const Path& path)
{
	doc_ = std::make_shared<pugi::xml_document>();
	auto ok = doc_->load_file(path.string().data());
	if (not ok)
		THROW(Error, "(Xml) Could not read file ", path, ": ", ok.description());
}

////////////////////////////////////////////////////////////////////////////////
void
Xml::write(const Path& path) const
{
	assert(doc_);
	doc_->save_file(path.string().data());
}
