#include "PageXmlParserLine.hpp"
#include "core/LineBuilder.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <utf8.h>

using namespace pcw;

static std::wstring
get_text_equiv_unicode(const Xml::Node& node);
static std::vector<Xml::Node>
get_glyph_nodes(const Xml::Node& node);
static std::vector<Xml::Node>
get_word_nodes(const Xml::Node& node);
static Box
get_coords_points(const Xml::Node& node);
static double
get_text_equiv_conf(const Xml::Node& node);

////////////////////////////////////////////////////////////////////////////////
PageXmlParserLine::PageXmlParserLine(const Xml::Node& line_node)
  : node_(line_node)
  , string_(get_text_equiv_unicode(line_node))
  , words_()
{
  parse();
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::end_wagner_fischer()
{
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::insert(size_t i, wchar_t c)
{
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::erase(size_t i)
{
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::set(size_t i, wchar_t c)
{
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
PageXmlParserLine::wstring() const
{
  return string_;
}

////////////////////////////////////////////////////////////////////////////////
std::string
PageXmlParserLine::string() const
{
  std::string str;
  utf8::utf32to8(string_.begin(), string_.end(), std::back_inserter(str));
  return str;
}

////////////////////////////////////////////////////////////////////////////////
LinePtr
PageXmlParserLine::line(int id) const
{
  LineBuilder builder;
  auto lbox = get_coords_points(node_);
  builder.set_id(id);
  builder.set_box(lbox);
  if (words_.empty()) {
    const auto conf = get_text_equiv_conf(node_);
    return builder.append(string_, lbox.right(), conf).build();
  }
  for (const auto& w : words_) {
    if (w.glyphs.empty()) {
      const auto conf = get_text_equiv_conf(w.node);
      const auto wbox = get_coords_points(w.node);
      builder.append(get_text_equiv_unicode(w.node), wbox.right(), conf);
    } else {
      for (const auto& g : w.glyphs) {
        const auto conf = get_text_equiv_conf(g.node);
        const auto gbox = get_coords_points(g.node);
        builder.append(get_text_equiv_unicode(g.node), gbox.right(), conf);
      }
    }
  }
  return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::parse()
{
  for (const auto word_node : get_word_nodes(node_)) {
    word w;
    w.node = word_node;
    size_t ppos = 0;
    if (not words_.empty()) {
      ppos = words_.back().end;
    }
    const auto wstr = get_text_equiv_unicode(w.node);
    const auto pos = string_.find(wstr, ppos);
    if (pos == std::wstring::npos) {
      // skip; don't throw
      continue;
    }
    w.begin = pos;
    w.end = pos + wstr.size();
    w.parse();
    words_.push_back(w);
  }
}

////////////////////////////////////////////////////////////////////////////////
void
PageXmlParserLine::word::parse()
{
  size_t i = this->begin;
  for (const auto glyph_node : get_glyph_nodes(this->node)) {
    glyph g;
    g.node = glyph_node;
    g.pos = i;
    this->glyphs.push_back(g);
    i++;
  }
}

////////////////////////////////////////////////////////////////////////////////
static std::vector<Xml::Node>
get_sub_nodes(const Xml::Node& node, const char* xpath)
{
  std::vector<Xml::Node> sub_nodes;
  for (auto sub : node.select_nodes(xpath)) {
    sub_nodes.push_back(sub.node());
  }
  return sub_nodes;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Xml::Node>
get_word_nodes(const Xml::Node& node)
{
  return get_sub_nodes(node, "./Word");
}

////////////////////////////////////////////////////////////////////////////////
std::vector<Xml::Node>
get_glyph_nodes(const Xml::Node& node)
{
  return get_sub_nodes(node, "./Glyph");
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
get_text_equiv_unicode(const Xml::Node& node)
{
  auto unicode = node.select_node("./TextEquiv/Unicode");
  auto str = unicode.node().child_value();
  auto n = strlen(str);
  std::wstring wstr;
  utf8::utf8to32(str, str + n, std::back_inserter(wstr));
  return wstr;
}

////////////////////////////////////////////////////////////////////////////////
double
get_text_equiv_conf(const Xml::Node& node)
{
  const auto conf =
    node.select_node("./TextEquiv").node().attribute("conf").value();
  try {
    return std::stod(conf);
  } catch (const std::invalid_argument&) {
    // ignore
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
Box
get_coords_points(const Xml::Node& node)
{
  Box box;
  auto points = node.select_node("./Coords").node().attribute("points").value();
  std::vector<std::string> ps, xy;
  boost::split(ps, points, [](char c) { return c == ' '; });
  for (const auto& p : ps) {
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
