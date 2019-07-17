#ifndef pcw_util_hpp__
#define pcw_util_hpp__

#include <boost/filesystem/path.hpp>
#include <stdexcept>

namespace pcw {
using Path = boost::filesystem::path;
class PageParser;
using PageParserPtr = std::unique_ptr<PageParser>;

const char *what(const std::exception &e) noexcept;
unsigned long genseed();
std::string gensessionid(size_t n);
std::string gensalt();
std::string genhash(const std::string &salt, const std::string &str);
bool check(const std::string &comb, const std::string &passwd);
std::string fix_windows_path(std::string path);

enum class FileType {
  Other,
  Img,
  AltoXml,
  AbbyyXml,
  Hocr,
  Llocs,
  Mets,
  PageXml,
  End,
};
std::string file_type_to_string(FileType type);
FileType file_type_from_string(const std::string &type);
FileType get_file_type(const Path &path);
FileType get_xml_file_type(const Path &path);
PageParserPtr make_page_parser(const Path &path);
PageParserPtr make_page_parser(FileType type, const Path &path);

std::string utf8(const std::wstring &str);
std::wstring utf8(const std::string &str);
void to_upper(std::string &str);
void to_upper(std::wstring &str) noexcept;
void to_lower(std::string &str);
void to_lower(std::wstring &str) noexcept;
enum class Capitalization { LowerCase, TitleCase, Capitalized };
Capitalization get_capitalization(const std::wstring &str) noexcept;
Capitalization get_capitalization(const std::string &str);
void apply_capitalization(Capitalization cap, std::wstring &str) noexcept;
void apply_capitalization(Capitalization cap, std::string &str) noexcept;

void hard_link_or_copy(const Path &from, const Path &to);
void hard_link_or_copy(const Path &from, const Path &to,
                       boost::system::error_code &ec) noexcept;
} // namespace pcw

#endif // pcw_util_hpp__
