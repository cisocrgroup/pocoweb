#ifndef pcw_util_hpp__
#define pcw_util_hpp__

#include <boost/filesystem/path.hpp>
#include <stdexcept>

namespace pcw {
using Path = boost::filesystem::path;
class PageParser;
using PageParserPtr = std::unique_ptr<PageParser>;

const char* what(const std::exception& e) noexcept;
unsigned long genseed();
std::string gensessionid(size_t n);
std::string gensalt();
std::string genhash(const std::string& salt, const std::string& str);
bool check(const std::string& comb, const std::string& passwd);
std::string fix_windows_path(std::string path);

enum class FileType {
	Other,
	Img,
	AltoXml,
	AbbyyXml,
	Hocr,
	Llocs,
	Mets,
	// Page,
};
std::string file_type_to_string(FileType type);
FileType file_type_from_string(const std::string& type);
FileType get_file_type(const Path& path);
FileType get_xml_file_type(const Path& path);
PageParserPtr make_page_parser(const Path& path);
PageParserPtr make_page_parser(FileType type, const Path& path);

std::string utf8(const std::wstring& str);
std::wstring utf8(const std::string& str);
}

#endif  // pcw_util_hpp__
