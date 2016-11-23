#include <sstream>
#include "BookView.hpp"
#include "Page.hpp"
#include "docxml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
namespace pcw {
	DocXmlNode& operator<<(DocXmlNode& j, const Page& page);
	DocXmlNode& operator<<(DocXmlNode& j, const Line& line);
	DocXmlNode& operator<<(DocXmlNode& j, const Line::Token& token);
	DocXmlNode& operator<<(DocXmlNode& j, const Box& Box);
}

////////////////////////////////////////////////////////////////////////////////
DocXml&
pcw::operator<<(DocXml& docxml, const BookView& view)
{
	docxml.doc.reset();
	DocXmlNode document{docxml.doc.append_child()};
	document.node.set_name("document");
	for (const auto& page: view) {
		assert(page);
		document << *page;
	}
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXml&
pcw::operator<<(DocXml& docxml, const Page& page)
{
	docxml.doc.reset();
	DocXmlNode document{docxml.doc.append_child()};
	document.node.set_name("document");
	document << page;
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
pcw::operator<<(DocXmlNode& node, const Page& page)
{
	DocXmlNode pnode{node.node.append_child()};
	pnode.node.set_name("page");
	pnode.node.append_attribute("imageFile").set_value(page.img.string().data());
	pnode.node.append_attribute("sourceFile").set_value(page.ocr.string().data());
	for (const auto& line: page) {
		assert(line);
		pnode << *line;
	}
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
pcw::operator<<(DocXmlNode& node, const Line& line)
{
	int id = 0;
	line.each_token([&node,&id,&line](const auto& token) {
		if (token.is_normal())
			node << token;
	});
	return node;
}

////////////////////////////////////////////////////////////////////////////////
static std::string
ext_id(const Line::Token& token)
{
	std::stringstream os;
	os << token.line->page()->book()->id()
	   << ":" << token.line->page()->id
	   << ":" << token.line->id()
	   << ":" << token.id;
	return os.str();
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
pcw::operator<<(DocXmlNode& node, const Line::Token& token)
{
	static const char* bools[] = {"false", "true"};
	DocXmlNode tnode{node.node.append_child()};
	tnode.node.set_name("token");
	tnode.node.append_attribute("token_id").set_value(token.unique_id());
	tnode.node.append_attribute("isCorrected").
		set_value(bools[(int)!!token.is_corrected()]);
	tnode.node.append_attribute("isNormal").
		set_value(bools[(int)!!token.is_normal()]);
	auto tmp = tnode.node.append_child();
	tmp.set_name("ext_id");
	tmp.append_child(pugi::node_pcdata).set_value(ext_id(token).data());

	tmp = tnode.node.append_child();
	tmp.set_name("wOCR_lc");
	tmp.append_child(pugi::node_pcdata).set_value(token.ocr_lc().data());

	tmp = tnode.node.append_child();
	tmp.set_name("wOCR");
	tmp.append_child(pugi::node_pcdata).set_value(token.ocr().data());

	tmp = tnode.node.append_child();
	tmp.set_name("wCorr");
	tmp.append_child(pugi::node_pcdata).set_value(token.cor().data());

	tnode << token.box;
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
pcw::operator<<(DocXmlNode& node, const Box& box)
{
	auto coord = node.node.append_child();
	coord.set_name("coord");
	coord.append_attribute("l").set_value(box.left());
	coord.append_attribute("t").set_value(box.top());
	coord.append_attribute("r").set_value(box.right());
	coord.append_attribute("b").set_value(box.bottom());
	return node;
}
