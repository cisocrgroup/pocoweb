#include <iostream>
#include <regex>
#include <utf8.h>
#include "core/Line.hpp"
#include "HocrParserLine.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::HocrParserLine(pugi::xml_node node)
	: chars_()
	, node_(node) 
	, needs_update_(false)
{
	init();
	const auto title = node_.attribute("title").value();
	const auto n = strlen(title);
	box = get_box(title, n);
}

////////////////////////////////////////////////////////////////////////////////
Line
HocrParserLine::line(int id) const
{
	Line line(id, box);
	for (const auto& c: chars_) {
		line.append(c.c, c.box.right(), c.conf);
	}
	return line;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring 
HocrParserLine::wstring() const 
{
	std::wstring res(chars_.size(), 0);
	std::transform(begin(chars_), end(chars_), begin(res), [](const auto& c) {
		return c.c;
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
std::string 
HocrParserLine::string() const 
{
	std::string res;
	res.reserve(chars_.size());
	std::for_each(begin(chars_), end(chars_), [&res](const auto& c) {
		const auto wc = c.c;
		utf8::utf32to8(&wc, &wc + 1, std::back_inserter(res));
	});
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::end_wagner_fischer()
{
	if (not needs_update_)
		return;
	auto last = node_.last_child();
// 	auto copy = node_.parent().insert_copy_before(node_, node_);
// 	for (auto& node: copy.children())
// 		node.parent().remove_child(node);

	const auto b = begin(chars_);
	const auto e = end(chars_);
	for (auto i = b; i != e; ) {
		auto eot = find_end_of_token(i, e);
		if (not i->is_space) {
			update_char(i, eot, node_);
		}
		i = eot;	
	}	

	for (auto n = node_.first_child(); n != last;) {
		auto next = n.next_sibling();
		n.parent().remove_child(n);
		n = next;
	}
	last.parent().remove_child(last);
	// node_.parent().remove_child(node_);
	// node_ = copy;
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::update_char(Iterator b, Iterator e, Node& parent)
{
	assert(b != e);
	Node node;
	if (not b->node) {
		node = parent.append_child("span");
	} else {
		node = parent.append_copy(b->node);
		node.set_name("span");
	}
	std::wstring wstr;
	double conf = 0;
	auto box = b->box;
	std::for_each(b, e, [&node,&wstr,&conf,&box](Char& c) {
		assert(not c.is_space);
		wstr.push_back(c.c);
		box += c.box;
		conf += c.conf;
		c.node = merge(node, c.node);
		
	});
	conf /= wstr.size();
	set_box(box, node);
	set_conf(conf, node);
	set_content(wstr, node);
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::insert(size_t pos, wchar_t c)
{
	// std::cerr << "(HocrParserLine) insert pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_.insert(begin(chars_) + pos, make_copy(chars_[pos], c));
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::erase(size_t pos)
{
	// std::cerr << "(HocrParserLine) erase pos: " << pos << "\n";
	assert(pos < chars_.size());
	chars_.erase(begin(chars_) + pos);	
	needs_update_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::set(size_t pos, wchar_t c)
{
	// std::cerr << "(HocrParserLine) set pos: " << pos << " " << c << "\n";
	assert(pos < chars_.size());
	chars_[pos].c = c;
	chars_[pos].is_space = isspace(c);
	needs_update_ = true;
}

     // <span class='ocr_line' id='line_1_1' title="bbox 297 121 1906 195; baseline 0.001 -23">
     // <span class='ocrx_word' id='word_1_1' title='bbox 297 143 360 175; x_wconf 81' lang='eng'>20</span> 
     // <span class='ocrx_word' id='word_1_2' title='bbox 925 129 999 177; x_wconf 70' lang='eng' dir='ltr'>De</span> 
     // <span class='ocrx_word' id='word_1_3' title='bbox 1041 122 1255 172; x_wconf 65' lang='eng' dir='ltr'>Homint.</span> 
     // <span class='ocrx_word' id='word_1_4' title='bbox 1700 121 1827 195; x_wconf 80' lang='eng' dir='ltr'>Cap.</span> 
     // <span class='ocrx_word' id='word_1_5' title='bbox 1855 140 1906 190; x_wconf 65' lang='eng'>5:.</span> 
////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init_string(const pugi::xml_node& node, bool space_before)
{
	assert(strcmp(node.name(), "String") == 0);

	const auto title = node.attribute("title").value();
	const auto n = strlen(title);
	const auto wc = get_conf(title, n);
	const auto box = get_box(title, n);
	const auto token = node.child_value();
	const auto len = strlen(token);

	std::wstring wstr;
	utf8::utf8to32(token, token + len, std::back_inserter(wstr));
	const auto boxes = box.split(wstr.size());
	assert(boxes.size() == wstr.size());
	if (space_before and not boxes.empty() and not chars_.empty()) {
		chars_.back().box.set_right(boxes.front().left());
	}
	for (auto i = 0U; i < wstr.size(); ++i) {
		chars_.emplace_back(wstr[i], node, wc, boxes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init_space(const Node& node) 
{
	auto title = node.attribute("title").value();
	auto n = strlen(title);
	auto token_box = get_box(title, n);
	Box box{
		token_box.right(),
		token_box.top(), 
		token_box.right() + 1, 
		token_box.bottom()
	};
	chars_.emplace_back(L' ', node, get_conf(title, n), box);
}

////////////////////////////////////////////////////////////////////////////////
void
HocrParserLine::init()
{
	bool span = false;
	for (const auto& node: node_.children()) {
		if (strcmp(node.name(), "span") == 0) {
			if (strcmp(node.attribute("class").value(), "ocrx_word") == 0) {
				if (span) {
					init_space(node);
				}
				init_string(node, span);
				span = true;
			}
		}
	}
}

// <span title="bbox 297 121 1906 195; baseline 0.001 -23">
////////////////////////////////////////////////////////////////////////////////
Box
HocrParserLine::get_box(const char* title, size_t n)
{
	static const std::regex bboxre{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+"))"};
	std::cmatch m;
	if (std::regex_search(title, title + n, m, bboxre)) {
		return Box{stoi(m[1]), stoi(m[2]), stoi(m[3]), stoi(m[4])};
	} else {
		return {};
	}
}	

// <span class='ocrx_word' ... title='bbox 1700 121 1827 195; x_wconf 80' lang='eng' dir='ltr'>Cap.</span> 
////////////////////////////////////////////////////////////////////////////////
double
HocrParserLine::get_conf(const char *title, size_t n)
{
	static const std::regex confre{R"(x_wconf\s+(\d+))"};
	std::cmatch m;
	if (std::regex_search(title, title + n, m, confre)) {
		auto p = static_cast<double>(std::stoi(m[1]));
		return p / 100.0;
	} else {
		return 0;
	}
}

// <span class='ocrx_word' ... title='bbox 1700 121 1827 195; x_wconf 80' lang='eng' dir='ltr'>Cap.</span> 
////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::set_box(const Box& box, Node& node)
{
	static const std::regex bboxre{R"(bbox\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+"))"};
	std::stringstream os;
	os << "bbox " 
	   << box.left() << " " 
	   << box.top() << " "
	   << box.right() << " "
	   << box.bottom();
	if (not node.attribute("title")) {
		node.append_attribute("title").set_value(os.str().data());
	} else {
		std::string title = node.attribute("title").value();
		auto repl = std::regex_replace(title, bboxre, os.str());
		node.attribute("title").set_value(repl.data());
	}
}

// <span title='bbox 1700 121 1827 195; x_wconf 80' lang='eng' dir='ltr'>Cap.</span> 
////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::set_conf(double conf, Node& node)
{
	static const std::regex confre{R"(x_wconf\s+(\d+))"};
	auto iconf = static_cast<int>(conf * 100);
	std::stringstream os;
	os << "x_wconf " << iconf;
	if (not node.attribute("title")) {
		node.append_attribute("title").set_value(os.str().data());
	} else {
		std::string title = node.attribute("title").value();
		auto repl = std::regex_replace(title, confre, os.str());
		node.attribute("title").set_value(repl.data());
	}
}

// <span title='bbox 1700 121 1827 195; x_wconf 80' lang='eng' dir='ltr'>Cap.</span> 
////////////////////////////////////////////////////////////////////////////////
void 
HocrParserLine::set_content(const std::wstring& str, Node& node)
{
	if (not node.first_child())
		node.append_child(pugi::node_pcdata);
	std::string ustr;
	ustr.reserve(str.size());
	utf8::utf32to8(begin(str), end(str), std::back_inserter(ustr));
	node.first_child().set_value(ustr.data());
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Iterator 
HocrParserLine::find_end_of_token(Iterator b, Iterator e) noexcept
{
	assert(b != e);
	if (b->is_space)
		return b + 1;
	else
		return std::find_if(b + 1, e, [](const auto& c) {return c.is_space;});
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Char 
HocrParserLine::make_copy(Char& c, wchar_t cc)
{
	auto split = c.box.split(2);
	auto copy = Char(cc, c.node, c.conf, split[0]);
	c.box = split[1];
	return copy;	
}

////////////////////////////////////////////////////////////////////////////////
HocrParserLine::Node 
HocrParserLine::merge(Node& a, const Node& b)
{
	if (a != b) { // don't merge the same node with itself
		for (const auto& attr: b.attributes()) {
			if (not a.attribute(attr.name())) 
				a.append_attribute(attr.name()).set_value(attr.value());
		}
	}
	return a;
}
