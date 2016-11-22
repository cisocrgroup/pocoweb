#include "BookView.hpp"
#include "Page.hpp"
#include "docxml.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static DocXmlNode& operator<<(DocXmlNode& j, const Page& page);
static DocXmlNode& operator<<(DocXmlNode& j, const Line& line);
static DocXmlNode& operator<<(DocXmlNode& j, const Line::Token& token);
static DocXmlNode& operator<<(DocXmlNode& j, const Box& Box);

////////////////////////////////////////////////////////////////////////////////
DocXml&
operator<<(DocXml& docxml, const BookView& project)
{
	docxml.doc.reset();
	DocXmlNode document{docxml.doc.append_child()};
	document.node.set_name("document");
	for (const auto& page: project) {
		assert(page);
		document << *page;
	}
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXml&
operator<<(DocXml& docxml, const Page& page)
{
	docxml.doc.reset();
	DocXmlNode document{docxml.doc.append_child()};
	document.node.set_name("document");
	document << page;
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
operator<<(DocXmlNode& node, const Page& page)
{
	for (const auto& line: page) {
		assert(line);
		node << *line;
	}
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
operator<<(DocXmlNode& node, const Line& line)
{
	line.each_token([&node](const auto& token) {node << token;});
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
operator<<(DocXmlNode& node, const Line::Token& token)
{
	node << token.box;
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode&
operator<<(DocXmlNode& node, const Box& box)
{
	return node;
}
