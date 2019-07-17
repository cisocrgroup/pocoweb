#include "util.hpp"
#include "parser/AbbyyXmlPageParser.hpp"
#include "parser/AltoXmlPageParser.hpp"
#include "parser/HocrPageParser.hpp"
#include "parser/OcropusLlocsPageParser.hpp"
#include "parser/PageXmlPageParser.hpp"
#include "utils/Error.hpp"
#include <algorithm>
#include <boost/filesystem/operations.hpp>
#include <cassert>
#include <crow/logging.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <openssl/sha.h>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unicode/uchar.h>
#include <unistd.h>
#include <utf8.h>

#ifndef PCW_WHAT_LEN
#define PCW_WHAT_LEN 1024
#endif // PCW_WHAT_LEN

// using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char *pcw::what(const std::exception &e) noexcept {
  static char buffer[PCW_WHAT_LEN];
  return strncpy(buffer, e.what(), PCW_WHAT_LEN);
}

////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/a/323302
// http://www.concentric.net/~Ttwang/tech/inthash.htm
static unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
  a = a - b;
  a = a - c;
  a = a ^ (c >> 13);
  b = b - c;
  b = b - a;
  b = b ^ (a << 8);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 13);
  a = a - b;
  a = a - c;
  a = a ^ (c >> 12);
  b = b - c;
  b = b - a;
  b = b ^ (a << 16);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 5);
  a = a - b;
  a = a - c;
  a = a ^ (c >> 3);
  b = b - c;
  b = b - a;
  b = b ^ (a << 10);
  c = c - a;
  c = c - b;
  c = c ^ (b >> 15);
  return c;
}

////////////////////////////////////////////////////////////////////////////////
unsigned long pcw::genseed() { return mix(clock(), time(NULL), getpid()); }

////////////////////////////////////////////////////////////////////////////////
static std::string dohash(const char *salt, size_t saltn, const char *str,
                          size_t strn) {
  // ERROR CHECKING!!
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, salt, saltn);
  SHA256_Update(&sha256, str, strn);
  SHA256_Final(hash, &sha256);

  std::stringstream ios;
  for (auto i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    ios << std::setw(2) << std::setfill('0') << std::hex
        << static_cast<int>(hash[i]);
  }
  return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string pcw::gensessionid(size_t n) {
  const auto seed = genseed();
  std::string id(n, 0);

  std::uniform_int_distribution<char> d('a', 'z');
  std::mt19937 gen(seed);
  for (auto &c : id)
    c = d(gen);
  return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string pcw::gensalt() {
  auto seed = genseed();
  std::uniform_int_distribution<unsigned int> d;
  std::mt19937 gen(seed);
  const auto n = d(gen);

  std::stringstream ios;
  ios << std::setw(sizeof(unsigned int)) << std::setfill('0') << std::hex << n;
  return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string pcw::genhash(const std::string &salt, const std::string &str) {
  return dohash(salt.data(), salt.size(), str.data(), str.size());
}

////////////////////////////////////////////////////////////////////////////////
bool pcw::check(const std::string &comb, const std::string &passwd) {
  auto ptr = std::find(comb.data(), comb.data() + comb.size(), '$');
  assert(ptr != comb.data() + comb.size());
  const auto n = std::distance(comb.data(), ptr);
  auto hash = dohash(comb.data(), n, passwd.data(), passwd.size());
  return strcmp(ptr + 1, hash.data()) == 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string pcw::fix_windows_path(std::string path) {
  const std::regex drive{R"(^[a-zA-Z]:\\+)"};
  path = std::regex_replace(path, drive, "/");
  std::replace(begin(path), end(path), '\\', '/');
  return path;
}

////////////////////////////////////////////////////////////////////////////////
pcw::FileType pcw::get_xml_file_type(const Path &path) {
  static const std::string abbyy{"http://www.abbyy.com"};
  static const std::string alto{"<alto"};
  static const std::string hocr{"<html"};
  static const std::string mets{"METS"};
  static const std::string page{"<PcGts"};

  std::ifstream is(path.string());
  if (not is.good())
    throw std::system_error(errno, std::system_category(), path.string());
  char buf[1024];
  is.read(buf, 1024);
  const auto n = is.gcount();
  if (std::search(buf, buf + n, begin(abbyy), end(abbyy)) != buf + n)
    return FileType::AbbyyXml;
  if (std::search(buf, buf + n, begin(alto), end(alto)) != buf + n)
    return FileType::AltoXml;
  if (std::search(buf, buf + n, begin(hocr), end(hocr)) != buf + n)
    return FileType::Hocr;
  if (std::search(buf, buf + n, begin(mets), end(mets)) != buf + n)
    return FileType::Mets;
  if (std::search(buf, buf + n, begin(page), end(page)) != buf + n)
    return FileType::PageXml;
  return FileType::Other;
}

////////////////////////////////////////////////////////////////////////////////
std::string pcw::file_type_to_string(FileType type) {
  switch (type) {
  case FileType::Other:
    return "Other";
  case FileType::Img:
    return "Image";
  case FileType::AltoXml:
    return "AltoXML";
  case FileType::AbbyyXml:
    return "AbbyyXML";
  case FileType::Hocr:
    return "hOCR";
  case FileType::Llocs:
    return "llocs";
  case FileType::Mets:
    return "METS/MOTS";
  default:
    assert(false);
    throw std::logic_error("Unreacheable code was reached!");
  }
}

////////////////////////////////////////////////////////////////////////////////
pcw::FileType pcw::file_type_from_string(const std::string &type) {
  if (type == "Other") {
    return FileType::Other;
  } else if (type == "Image") {
    return FileType::Img;
  } else if (type == "AltoXML") {
    return FileType::AltoXml;
  } else if (type == "AbbyyXML") {
    return FileType::AbbyyXml;
  } else if (type == "hOCR") {
    return FileType::Hocr;
  } else if (type == "llocs") {
    return FileType::Llocs;
  } else if (type == "METS/MOTS") {
    return FileType::Mets;
  }
  throw std::runtime_error("Invalid FileType: " + type);
}

////////////////////////////////////////////////////////////////////////////////
pcw::FileType pcw::get_file_type(const Path &path) {
  static const std::regex hocr{
      R"(\.((html?)|(hocr))$)", std::regex_constants::icase};
  static const std::regex xml{R"(\.xml$)", std::regex_constants::icase};
  static const std::regex llocs{R"(\.llocs$)"};
  static const std::regex img{
      R"(\.((png)|(jpe?g)|(tiff?))$)", std::regex_constants::icase};
  auto str = path.string();
  if (std::regex_search(str, img))
    return FileType::Img;
  if (std::regex_search(str, llocs))
    return FileType::Llocs;
  if (std::regex_search(str, xml))
    return get_xml_file_type(path);
  if (std::regex_search(str, hocr))
    return FileType::Hocr;
  return FileType::Other;
}

////////////////////////////////////////////////////////////////////////////////
pcw::PageParserPtr pcw::make_page_parser(const Path &ocr) {
  auto type = get_file_type(ocr);
  return make_page_parser(type, ocr);
}

////////////////////////////////////////////////////////////////////////////////
pcw::PageParserPtr pcw::make_page_parser(FileType type, const Path &ocr) {
  switch (type) {
  case FileType::Hocr:
    return std::make_unique<HocrPageParser>(ocr);
  case FileType::AltoXml:
    return std::make_unique<AltoXmlPageParser>(ocr);
  case FileType::AbbyyXml:
    return std::make_unique<AbbyyXmlPageParser>(ocr);
  case FileType::Llocs:
    return std::make_unique<OcropusLlocsPageParser>(ocr);
  case FileType::PageXml:
    return std::make_unique<PageXmlPageParser>(ocr);
  default:
    THROW(BadRequest, "(util) Cannot parse file: ", ocr);
  }
}
////////////////////////////////////////////////////////////////////////////////
std::string pcw::utf8(const std::wstring &str) {
  std::string res;
  utf8::utf32to8(begin(str), end(str), std::back_inserter(res));
  return res;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring pcw::utf8(const std::string &str) {
  std::wstring res;
  utf8::utf8to32(begin(str), end(str), std::back_inserter(res));
  return res;
}

////////////////////////////////////////////////////////////////////////////////
void pcw::to_upper(std::string &str) {
  auto wstr = utf8(str);
  to_upper(wstr);
  str = utf8(wstr);
}

////////////////////////////////////////////////////////////////////////////////
void pcw::to_upper(std::wstring &str) noexcept {
  for (auto &c : str)
    c = u_toupper(c);
}

////////////////////////////////////////////////////////////////////////////////
void pcw::to_lower(std::string &str) {
  auto wstr = utf8(str);
  to_lower(wstr);
  str = utf8(wstr);
}

////////////////////////////////////////////////////////////////////////////////
void pcw::to_lower(std::wstring &str) noexcept {
  for (auto &c : str)
    c = u_tolower(c);
}

////////////////////////////////////////////////////////////////////////////////
static void set_capitalization_vars(wchar_t c, bool &firstchar,
                                    bool &firstupper, bool &allupper,
                                    bool &alllower) noexcept {
  const bool upper = (u_charType(c) == U_UPPERCASE_LETTER);
  if (upper and firstchar)
    firstupper = true;
  if (not upper)
    allupper = false;
  if (upper)
    alllower = false;
  firstchar = false;
}

////////////////////////////////////////////////////////////////////////////////
static pcw::Capitalization do_get_capitalization(bool firstupper, bool allupper,
                                                 bool alllower) noexcept {
  if (allupper)
    return pcw::Capitalization::TitleCase;
  else if (alllower)
    return pcw::Capitalization::LowerCase;
  else if (firstupper)
    return pcw::Capitalization::Capitalized;
  else // first char is not upper, not all chars are lower case
    return pcw::Capitalization::TitleCase;
}

////////////////////////////////////////////////////////////////////////////////
pcw::Capitalization pcw::get_capitalization(const std::wstring &str) noexcept {
  bool firstchar = true;
  bool firstupper = false;
  bool allupper = true;
  bool alllower = true;
  for (auto c : str) {
    set_capitalization_vars(c, firstchar, firstupper, allupper, alllower);
  }
  return do_get_capitalization(firstupper, allupper, alllower);
}

////////////////////////////////////////////////////////////////////////////////
pcw::Capitalization pcw::get_capitalization(const std::string &str) {
  bool firstchar = true;
  bool firstupper = false;
  bool allupper = true;
  bool alllower = true;
  const auto b = begin(str);
  const auto e = end(str);
  using it = utf8::unchecked::iterator<std::string::const_iterator>;
  std::for_each(it(b), it(e), [&](wchar_t c) {
    set_capitalization_vars(c, firstchar, firstupper, allupper, alllower);
  });
  return do_get_capitalization(firstupper, allupper, alllower);
}

////////////////////////////////////////////////////////////////////////////////
void pcw::apply_capitalization(Capitalization cap, std::wstring &str) noexcept {
  if (cap == Capitalization::LowerCase) {
    to_lower(str);
  } else if (cap == Capitalization::TitleCase) {
    to_upper(str);
  } else {
    to_lower(str);
    if (not str.empty()) {
      str[0] = u_toupper(str[0]);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void pcw::apply_capitalization(Capitalization cap, std::string &str) noexcept {
  std::wstring tmp = utf8(str);
  apply_capitalization(cap, tmp);
  str = utf8(tmp);
}

////////////////////////////////////////////////////////////////////////////////
void pcw::hard_link_or_copy(const Path &from, const Path &to) {
  boost::system::error_code ec;
  hard_link_or_copy(from, to, ec);
  if (ec) {
    throw std::system_error(ec.value(), std::system_category(), to.string());
  }
}

////////////////////////////////////////////////////////////////////////////////
void pcw::hard_link_or_copy(const Path &from, const Path &to,
                            boost::system::error_code &ec) noexcept {
  boost::filesystem::create_hard_link(from, to, ec);
  if (not ec || ec.value() == boost::system::errc::file_exists /*EEXISTS*/) {
    ec = boost::system::error_code();
  }
  // could not create hard link; try copy
  CROW_LOG_WARNING << "(hard_link_or_copy) Could not create hardlink from "
                   << from << " to " << to << ": " << ec.message();
  boost::filesystem::copy_file(from, to, ec);
}
