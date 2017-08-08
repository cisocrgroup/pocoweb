#include "docxml.hpp"
#include <sstream>
#include "Profile.hpp"
#include "core/Page.hpp"
#include "core/Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
struct DocXmlNode : public pugi::xml_node {
       private:
	DocXmlNode(const pugi::xml_node& node,
		   const Profile::Suggestions* suggs)
	    : pugi::xml_node(node), suggestions(suggs) {}

       public:
	static DocXmlNode append_new(DocXmlNode& other) {
		return DocXmlNode(other.append_child(), other.suggestions);
	}
	static DocXmlNode append_new(DocXml& other) {
		return DocXmlNode(other.append_child(), other.suggestions);
	}
	const Profile::Suggestions* suggestions;
};

////////////////////////////////////////////////////////////////////////////////
static void append_candidates(pugi::xml_node& node, const Token& token,
			      const Profile::Suggestions& suggs);

////////////////////////////////////////////////////////////////////////////////
namespace pcw {
DocXmlNode& operator<<(DocXmlNode& j, const Page& page);
DocXmlNode& operator<<(DocXmlNode& j, const Line& line);
DocXmlNode& operator<<(DocXmlNode& j, const Token& token);
DocXmlNode& operator<<(DocXmlNode& j, const Box& Box);
}

////////////////////////////////////////////////////////////////////////////////
DocXml& pcw::operator<<(DocXml& docxml, const Profile& profile) {
	docxml.suggestions = &profile.suggestions();
	return docxml << profile.book();
}

////////////////////////////////////////////////////////////////////////////////
DocXml& pcw::operator<<(DocXml& docxml, const Project& view) {
	docxml.reset();
	auto document = DocXmlNode::append_new(docxml);
	document.set_name("document");
	for (const auto& page : view) {
		assert(page);
		document << *page;
	}
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXml& pcw::operator<<(DocXml& docxml, const Page& page) {
	auto document = DocXmlNode::append_new(docxml);
	document.set_name("document");
	document << page;
	return docxml;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode& pcw::operator<<(DocXmlNode& node, const Page& page) {
	auto pnode = DocXmlNode::append_new(node);
	pnode.set_name("page");
	pnode.append_attribute("imageFile").set_value(page.img.string().data());
	pnode.append_attribute("sourceFile")
	    .set_value(page.ocr.string().data());
	for (const auto& line : page) {
		assert(line);
		pnode << *line;
	}
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode& pcw::operator<<(DocXmlNode& node, const Line& line) {
	int id = 0;
	line.each_token([&node, &id, &line](const auto& token) {
		if (token.is_normal()) node << token;
	});
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode& pcw::operator<<(DocXmlNode& node, const Token& token) {
	static const char* bools[] = {"false", "true"};
	auto tnode = DocXmlNode::append_new(node);
	tnode.set_name("token");
	tnode.append_attribute("token_id").set_value(token.unique_id());
	tnode.append_attribute("isCorrected")
	    .set_value(bools[(int)!!token.is_fully_corrected()]);
	tnode.append_attribute("isNormal")
	    .set_value(bools[(int)!!token.is_normal()]);
	auto tmp = tnode.append_child();
	tmp.set_name("ext_id");
	tmp.append_child(pugi::node_pcdata)
	    .set_value(std::to_string(token.unique_id()).data());

	tmp = tnode.append_child();
	tmp.set_name("wOCR_lc");
	tmp.append_child(pugi::node_pcdata).set_value(token.ocr_lc().data());

	tmp = tnode.append_child();
	tmp.set_name("wOCR");
	tmp.append_child(pugi::node_pcdata).set_value(token.ocr().data());

	tmp = tnode.append_child();
	tmp.set_name("wCorr");
	if (token.is_fully_corrected())
		tmp.append_child(pugi::node_pcdata)
		    .set_value(token.cor().data());

	if (node.suggestions)
		append_candidates(tnode, token, *node.suggestions);

	tnode << token.box;
	return node;
}

////////////////////////////////////////////////////////////////////////////////
DocXmlNode& pcw::operator<<(DocXmlNode& node, const Box& box) {
	auto coord = node.append_child();
	coord.set_name("coord");
	coord.append_attribute("l").set_value(box.left());
	coord.append_attribute("t").set_value(box.top());
	coord.append_attribute("r").set_value(box.right());
	coord.append_attribute("b").set_value(box.bottom());
	return node;
}

////////////////////////////////////////////////////////////////////////////////
void append_candidates(pugi::xml_node& node, const Token& token,
		       const Profile::Suggestions& suggs) {
	auto s = suggs.find(token);
	if (s == end(suggs)) return;
	for (const auto& c : s->second) {
		auto tmp = node.append_child();
		tmp.set_name("cand");
		std::string expr = c.cor() + ":" + c.explanation_string() +
				   ",voteWeight=" + std::to_string(c.weight()) +
				   ",levDistance=" + std::to_string(c.lev());
		tmp.append_child(pugi::node_pcdata).set_value(expr.data());
	}
}
