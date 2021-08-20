#include <fstream>
#include "pugixml.hpp"
#include "utils/Error.hpp"
#include "Xml.hpp"

using namespace pcw;

struct xml_writer: pugi::xml_writer {
	xml_writer(std::ostream& out): out_(out) {}
	std::ostream& out_;
	virtual void write(const void* data, size_t n) override;
};

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
	std::ofstream out(path.string());
	if (not out.good()) {
		THROW(Error, "(Xml) Cannot write file ", path);
	}
	auto w = xml_writer{out};
	auto format = pugi::format_no_empty_element_tags | pugi::format_indent;
	doc_->print(w, " ", format);
}

////////////////////////////////////////////////////////////////
// We need to do this to fix pugixml's handling of whitespace //
// whitespace character nodes in ABBYY-XML files.             //
////////////////////////////////////////////////////////////////
void
xml_writer::write(const void* data, size_t n)
{
	auto chars = (const char*) data;
	void *pos;
	while ((pos = memmem(chars, n, "></charParams", 13)) != NULL) {
		auto tmp = (const char*) pos;
		auto len = tmp - chars;
		out_.write(chars, len);
		out_.write("> </charParams", 14);
		chars = tmp + 13;
		n = n - (13 + len);
	}
	out_.write(chars, n);
}
