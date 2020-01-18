#include "PageXmlParserLine.hpp"
#include "core/LineBuilder.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sstream>
#include <utf8.h>

#define TEXT_REGION "TextRegion"
#define UNICODE "Unicode"
#define TEXT_EQUIV "TextEquiv"
#define INDEX "index"
#define CONF "conf"
#define POINTS "points"
#define WORD "Word"

using namespace pcw;

static std::wstring child_wstring(const Xml::Node &node);
static Xml::Node get_last_text_equiv(const Xml::Node &node);
static std::vector<Xml::Node> get_glyph_nodes(const Xml::Node &node);
static std::vector<Xml::Node> get_word_nodes(const Xml::Node &node);
static Box get_coords_points(const Xml::Node &node);
static double get_text_equiv_conf(const Xml::Node &node);
static std::vector<std::string> fields(const std::string &str);

////////////////////////////////////////////////////////////////////////////////
PageXmlParserLine::PageXmlParserLine(const Xml::Node &line_node, bool first)
    : node_(line_node),
      string_(get_text_equiv_unicode(get_last_text_equiv(line_node), 0)),
      words_(), first_(first) {
  parse();
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::end_wagner_fischer() {
  const auto line = string();
  add_corrections_to_line(line);
  add_corrections_to_words(line);
  add_corrections_to_regions(line);
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::add_corrections_to_line(const std::string &cor) {
  node_.append_child(TEXT_EQUIV)
      .append_child(UNICODE)
      .append_child(pugi::node_pcdata)
      .set_value(cor.data());
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::add_corrections_to_words(const std::string &cor) {
  const auto words = fields(cor);
  size_t i = 0;
  for (auto &word : node_.children(WORD)) {
    if (i >= words.size()) {
      word.append_child(TEXT_EQUIV)
          .append_child(UNICODE)
          .append_child(pugi::node_pcdata)
          .set_value("");
      continue;
    }
    word.append_child(TEXT_EQUIV)
        .append_child(UNICODE)
        .append_child(pugi::node_pcdata)
        .set_value(words[i].data());
    i++;
  }
  for (; i < words.size(); i++) { // add additional words
    auto w = node_.append_child(WORD);
    w.append_child(TEXT_EQUIV)
        .append_child(UNICODE)
        .append_child(pugi::node_pcdata)
        .set_value("");
    w.append_child(TEXT_EQUIV)
        .append_child(UNICODE)
        .append_child(pugi::node_pcdata)
        .set_value(words[i].data());
  }
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::add_corrections_to_regions(const std::string &cor) {
  if (first_) {
    for (auto p = node_.parent(); p and strcmp(p.name(), TEXT_REGION) == 0;
         p = p.parent()) {
      p.append_child(TEXT_EQUIV)
          .append_child(UNICODE)
          .append_child(pugi::node_pcdata)
          .set_value(cor.data());
    }
    return;
  }
  for (auto p = node_.parent(); p and strcmp(p.name(), TEXT_REGION) == 0;
       p = p.parent()) {
    for (auto child : p.children(TEXT_EQUIV)) {
      if (child.next_sibling(TEXT_EQUIV)) { // not the last text equiv
        continue;
      }
      std::string region = child.child(UNICODE).text().get();
      region = region + "\n" + cor;
      child.child(UNICODE).text().set(region.data());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::insert(size_t i, wchar_t c) { string_.insert(i, 1, c); }

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::erase(size_t i) { string_.erase(i); }

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::set(size_t i, wchar_t c) { string_[i] = c; }

////////////////////////////////////////////////////////////////////////////////
std::wstring PageXmlParserLine::wstring() const { return string_; }

////////////////////////////////////////////////////////////////////////////////
std::string PageXmlParserLine::string() const {
  std::string str;
  utf8::utf32to8(string_.begin(), string_.end(), std::back_inserter(str));
  return str;
}

////////////////////////////////////////////////////////////////////////////////
LinePtr PageXmlParserLine::line(int id) const {
  LineBuilder builder;
  auto lbox = get_coords_points(node_);
  builder.set_id(id);
  builder.set_box(lbox);
  if (words_.empty() or not checkWordBoxes()) {
    const auto conf = get_text_equiv_conf(get_last_text_equiv(node_));
    return builder.append(string_, lbox.right(), conf).build();
  }
  for (auto i = 0U; i < words_.size(); i++) {
    if (words_[i].glyphs.empty()) {
      const auto conf =
          get_text_equiv_conf(get_last_text_equiv(words_[i].word));
      const auto wbox = get_coords_points(words_[i].word);
      builder.append(
          get_text_equiv_unicode(get_last_text_equiv(words_[i].word), 0),
          wbox.right(), conf);
    } else {
      for (const auto &g : words_[i].glyphs) {
        const auto conf = get_text_equiv_conf(get_last_text_equiv(g.glyph));
        const auto gbox = get_coords_points(g.glyph);

        builder.append(get_text_equiv_unicode(get_last_text_equiv(g.glyph), 0),
                       gbox.right(), conf);
      }
    }
    // if there is a word after this (at pos i+1 in the vector) append ' '
    if ((i + 1) < words_.size()) {
      const auto nbox = get_coords_points(words_[i + 1].word);
      const auto conf =
          get_text_equiv_conf(get_last_text_equiv(words_[i + 1].word));
      builder.append(' ', nbox.left(), conf);
    }
  }
  return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
template<class T> bool checkBoxes(const std::vector<T>& ts) {
  Box beforebb;
  for (size_t i = 0; i < ts.size(); i++) {
	auto curbb = get_coords_points(ts[i].node());
	if (i > 0) {
	  if (beforebb.right() > curbb.left()) {
		return false;
	  }
	}
	beforebb = curbb; 
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool PageXmlParserLine::checkWordBoxes() const {
  return checkBoxes(words_);
}

////////////////////////////////////////////////////////////////////////////////
bool PageXmlParserLine::checkGlyphBoxes(const word& w) const {
  return checkBoxes(w.glyphs);
}

////////////////////////////////////////////////////////////////////////////////
void PageXmlParserLine::parse() {
  for (const auto word_node : get_word_nodes(node_)) {
    word w;
    w.word = word_node;
    for (const auto glyph_node : get_glyph_nodes(w.word)) {
      glyph g;
      g.glyph = glyph_node;
      w.glyphs.push_back(g);
    }
    words_.push_back(w);
  }
}

////////////////////////////////////////////////////////////////////////////////
static std::vector<Xml::Node> get_sub_nodes(const Xml::Node &node,
                                            const char *xpath) {
  std::vector<Xml::Node> sub_nodes;
  for (auto sub : node.select_nodes(xpath)) {
    sub_nodes.push_back(sub.node());
  }
  return sub_nodes;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Xml::Node> get_word_nodes(const Xml::Node &node) {
  return get_sub_nodes(node, "./*[local-name()='Word']");
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Xml::Node> get_glyph_nodes(const Xml::Node &node) {
  return get_sub_nodes(node, "./*[local-name()='Glyph']");
}

////////////////////////////////////////////////////////////////////////////////
Xml::Node get_last_text_equiv(const Xml::Node &node) {
  auto nodes = node.select_nodes("./*[local-name()='TextEquiv']");
  Xml::Node last_node;
  int last_index = 0;
  for (const auto n : nodes) {
    const auto i = n.node().attribute(INDEX).as_int();
    if (i > last_index) {
      last_index = i;
      last_node = n.node();
    } else if (i == 0) { // no index attribute or i == 0 (we do not care)
      last_node = n.node();
    }
  }
  return last_node;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring PageXmlParserLine::get_text_equiv_unicode(const Xml::Node &node,
                                                       int i) const {
  auto unicodes = node.select_nodes("./*[local-name()='Unicode']");
  if (unicodes.size() != 1) {
    return L"";
    // std::string id = node_.attribute("id").value();
    // std::string line;
    // utf8::utf32to8(string_.begin(), string_.end(), std::back_inserter(line));
    // throw std::runtime_error(
    //     "invalid pagexml: invalid unicode index: " + std::to_string(i) +
    //     " at line: " + line + " id: " + id);
  }
  const auto res = child_wstring(unicodes[i].node());
  return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring child_wstring(const Xml::Node &node) {
  auto str = node.child_value();
  auto n = strlen(str);
  std::wstring wstr;
  utf8::utf8to32(str, str + n, std::back_inserter(wstr));
  return wstr;
}

////////////////////////////////////////////////////////////////////////////////
double get_text_equiv_conf(const Xml::Node &node) {
  const auto conf = node.attribute(CONF).value();
  try {
    return std::stod(conf);
  } catch (const std::invalid_argument &) {
    // ignore
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
Box get_coords_points(const Xml::Node &node) {
  Box box;
  auto points = node.select_node("./*[local-name()='Coords']")
                    .node()
                    .attribute(POINTS)
                    .value();
  std::vector<std::string> ps, xy;
  boost::split(ps, points, [](char c) { return c == ' '; });
  for (const auto &p : ps) {
    xy.clear();
    boost::split(xy, p, [](char c) { return c == ','; });
    if (xy.size() != 2) {
      continue;
    }
    const auto x = std::stoi(xy[0]);
    const auto y = std::stoi(xy[1]);
    if (box.is_empty()) {
      box = Box(x, y, x, y);
    } else {
      box += Box(x, y, x, y);
    }
  }
  return box;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> fields(const std::string &str) {
  std::stringstream ss(str);
  std::string field;
  std::vector<std::string> fields;
  while (ss >> field) {
    fields.push_back(field);
  }
  return fields;
}
