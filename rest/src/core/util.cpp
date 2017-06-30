#include <cstring>
#include <cassert>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <random>
#include <unistd.h>
#include <regex>
#include <openssl/sha.h>
#include "utils/Error.hpp"
#include "util.hpp"
#include "parser/AltoXmlPageParser.hpp"
#include "parser/HocrPageParser.hpp"
#include "parser/OcropusLlocsPageParser.hpp"
#include "parser/AbbyyXmlPageParser.hpp"

#ifndef PCW_WHAT_LEN
#define PCW_WHAT_LEN 1024
#endif // PCW_WHAT_LEN

// using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char*
pcw::what(const std::exception& e) noexcept
{
	static char buffer[PCW_WHAT_LEN];
	return strncpy(buffer, e.what(), PCW_WHAT_LEN);
}

////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/a/323302
// http://www.concentric.net/~Ttwang/tech/inthash.htm
static unsigned long
mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

////////////////////////////////////////////////////////////////////////////////
unsigned long
pcw::genseed()
{
	return mix(clock(), time(NULL), getpid());
}

////////////////////////////////////////////////////////////////////////////////
static std::string
dohash(const char *salt, size_t saltn, const char *str, size_t strn)
{
	// ERROR CHECKING!!
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, salt, saltn);
	SHA256_Update(&sha256, str, strn);
	SHA256_Final(hash, &sha256);

	std::stringstream ios;
	for (auto i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		ios << std::setw(2)
		    << std::setfill('0')
		    << std::hex
		    << static_cast<int>(hash[i]);
	}
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::gensessionid(size_t n)
{
	auto seed = genseed();
	std::string id(n, 0);

	std::uniform_int_distribution<char> d('a', 'z');
	std::mt19937 gen(seed);
	for (auto& c: id)
		c = d(gen);
	return id;
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::gensalt()
{
	auto seed = genseed();
	std::uniform_int_distribution<unsigned int> d;
	std::mt19937 gen(seed);
	const auto n = d(gen);

	std::stringstream ios;
	ios << std::setw(sizeof(unsigned int))
	    << std::setfill('0')
	    << std::hex
	    << n;
	return ios.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::genhash(const std::string& salt, const std::string& str)
{
	return dohash(salt.data(), salt.size(), str.data(), str.size());
}

////////////////////////////////////////////////////////////////////////////////
bool
pcw::check(const std::string& comb, const std::string& passwd)
{
	auto ptr = std::find(comb.data(), comb.data() + comb.size(), '$');
	assert(ptr != comb.data() + comb.size());
	const auto n = std::distance(comb.data(), ptr);
	auto hash = dohash(comb.data(), n, passwd.data(), passwd.size());
	return strcmp(ptr + 1, hash.data()) == 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::fix_windows_path(std::string path)
{
	const std::regex drive{R"(^[a-zA-Z]:\\+)"};
	path = std::regex_replace(path, drive, "/");
	std::replace(begin(path), end(path),  '\\', '/');
	return path;
}

////////////////////////////////////////////////////////////////////////////////
pcw::FileType
pcw::get_xml_file_type(const Path& path)
{
	static const std::string abbyy{"http://www.abbyy.com"};
	static const std::string alto{"<alto"};
	static const std::string hocr{"<html"};
	static const std::string mets{"METS"};

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
	return FileType::Other;
}

////////////////////////////////////////////////////////////////////////////////
std::string
pcw::file_type_to_string(FileType type)
{
	switch (type) {
	case FileType::Other:
		return "Other";
	case FileType::Img:
		return "Image";
	case FileType::AltoXml:
		return "AltoXML";
	case FileType::AbbyyXml:
		return	"AbbyyXML";
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
pcw::FileType
pcw::get_file_type(const Path& path)
{
	static const std::regex hocr{
		R"(\.((html?)|(hocr))$)",
		std::regex_constants::icase
	};
	static const std::regex xml{R"(\.xml$)", std::regex_constants::icase};
	static const std::regex llocs{R"(\.llocs$)"};
	static const std::regex img{
		R"(\.((png)|(jpe?g)|(tiff?))$)",
		std::regex_constants::icase
	};
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
pcw::PageParserPtr
pcw::make_page_parser(const Path& ocr)
{
	auto type = get_file_type(ocr);
	return make_page_parser(type, ocr);
}

////////////////////////////////////////////////////////////////////////////////
pcw::PageParserPtr
pcw::make_page_parser(FileType type, const Path& ocr)
{
	switch (type) {
	case FileType::Hocr:
		return std::make_unique<HocrPageParser>(ocr);
	case FileType::AltoXml:
		return std::make_unique<AltoXmlPageParser>(ocr);
	case FileType::AbbyyXml:
		return std::make_unique<AbbyyXmlPageParser>(ocr);
	case FileType::Llocs:
		return std::make_unique<OcropusLlocsPageParser>(ocr);
	default:
		THROW(BadRequest, "(util) Cannot parse file: ", ocr);
	}
}

