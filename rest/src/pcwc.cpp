#include <boost/variant.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include <curl/curl.h>
#include <crow/json.h>
#include "core/Error.hpp"
#include "core/TmpDir.hpp"

using namespace pcw;

struct CurlFree {
	void operator()(char* str) const noexcept {
		curl_free(str);
	}
};
using CurlStr = std::unique_ptr<char, CurlFree>;

struct Address {
	int b, e;
};

struct CVersion {};
struct CQuit {};
struct CBooks {};
struct CBook {int id;};
struct CPage {int id;};
struct CNextPage {int ofs;};
struct CPrevPage {int ofs;};
struct CSetBook {std::string key, val;};
struct CChangeFile {std::string file;};
struct CChangeStr {std::string str;};
struct CFirstPage {};
struct CLastPage {};
struct CFirstLine {};
struct CLastLine {};
struct CNextLine {int ofs;};
struct CPrevLine {int ofs;};
struct CPrint {int b, e; bool cor;};
struct CInfo {};
struct CUpload {std::string what;};
struct CError {std::string what;};

struct LinePair {
	LinePair(std::string c, std::string o)
		: cor(std::move(c))
		, ocr(std::move(o))
	{}
	std::string cor, ocr;
};

using Command = boost::variant<
	CVersion,
	CBooks,
	CUpload,
	CBook,
	CPage,
	CSetBook,
	CChangeFile,
	CChangeStr,
	CNextPage,
	CPrevPage,
	CFirstPage,
	CLastPage,
	CFirstLine,
	CLastLine,
	CNextLine,
	CPrevLine,
	CPrint,
	CInfo,
	CQuit,
	CError
>;

static char EBUF[CURL_ERROR_SIZE + 1];
static size_t xwrite(char *ptr, size_t size, size_t nmemb, void *userdata);
static size_t xread(void *ptr, size_t size, size_t nmemb, void *userdata);

struct Ed: boost::static_visitor<void> {
	Ed(int argc, char* argv[]);
	~Ed() noexcept;

	void login();
	void perform() const;
	void get(const std::string& url);
	void post(const std::string& url);
	void put(const std::string& url);

	void operator()(const std::string& line);
	void operator()(CVersion);
	void operator()(CQuit);
	void operator()(CBooks);
	void operator()(const CSetBook& set);
	void operator()(const CChangeFile& cf);
	void operator()(const CChangeStr& cf);
	void operator()(const CUpload& c);
	void operator()(CBook b);
	void operator()(CPage p);
	void operator()(CNextPage np);
	void operator()(CPrevPage pp);
	void operator()(CFirstPage);
	void operator()(CLastPage);
	void operator()(CNextLine nl);
	void operator()(CPrevLine pl);
	void operator()(CFirstLine);
	void operator()(CLastLine);
	void operator()(const CPrint& p) const;
	void operator()(CInfo) const;
	void operator()(const CError& error) const;
	void read_book();
	void read_page();

	static Command parse(const std::string& line);

	std::string host, user, pass, buffer;
	std::ifstream is;
	TmpDir tmpdir;
	Path cookiefile;
	std::vector<LinePair> page;
	CURL* curl;
	int port, bookid, pageid, lineid, npages;
	bool done;
};

////////////////////////////////////////////////////////////////////////////////
Ed::Ed(int argc, char* argv[])
	: host()
	, user()
	, pass()
	, buffer()
	, is()
	, tmpdir()
	, cookiefile()
	, page()
	, curl()
	, port()
	, bookid()
	, pageid()
	, lineid()
	, npages()
	, done(false)
{
	if (argc != 5)
		THROW(Error, "Usage: ", argv[0], " <host> <port> <user> <pass>");
	if (std::stoi(argv[2]) <= 0)
		THROW(Error, "Expceted port > 0, got: ", argv[2]);
	host = argv[1];
	port = std::stoi(argv[2]);
	user = argv[3];
	pass = argv[4];
	cookiefile = tmpdir / "cookies.txt";
	curl = curl_easy_init();
	if (not curl)
		THROW(Error, "Could not initialize curl");

	curl_easy_setopt(curl, CURLOPT_PORT, port);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, EBUF);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile.string().data());
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiefile.string().data());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &xwrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &xread);
	curl_easy_setopt(curl, CURLOPT_READDATA, &is);
}

////////////////////////////////////////////////////////////////////////////////
Ed::~Ed() noexcept
{
	curl_easy_cleanup(curl);
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::login()
{
	auto url = "http://" + host + "/login/user/" + user + "/pass/" + pass;
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	get(url);
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::get(const std::string& url)
{
	assert(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_PUT, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 0);
	buffer.clear();
	perform();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::post(const std::string& url)
{
	assert(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
	curl_easy_setopt(curl, CURLOPT_PUT, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	buffer.clear();
	perform();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::put(const std::string& url)
{
	assert(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 0);
	curl_easy_setopt(curl, CURLOPT_PUT, 1);
	buffer.clear();
	perform();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::perform() const
{
	assert(curl);
	if (curl_easy_perform(curl) != CURLE_OK)
		THROW(Error, "curl_easy_perform(): ", EBUF);
	long http_code = 0;
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
	switch (http_code) {
	case 200:
	case 201:
		break;
	default:
		THROW(Error, "Got answer: ", http_code);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const std::string& line)
{
	auto command = parse(line);
	boost::apply_visitor(*this, command);
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CVersion)
{
	assert(curl);
	auto url = "http://" + host + "/api-version";
	buffer.clear();
	get(url);
	auto json = crow::json::load(buffer);
	std::cout << json["version"] << "\n";
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CBooks)
{
	static const char* type[] = {"package", "book"};
	assert(curl);
	auto url = "http://" + host + "/books";
	buffer.clear();
	get(url);
	auto json = crow::json::load(buffer);
	for (const auto& book: json["books"]) {
		std::cout << book["id"] << " "
			  << book["pages"] << " ("
			  << type[!!book["isBook"]] << ") "
			  << book["title"] << " "
			  << book["author"] << " "
			  << book["year"] << "\n";
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CUpload& c)
{
	assert(curl);
	is.open(c.what, std::ifstream::ate);
	if (not is.good())
		throw std::system_error(errno, std::system_category(), c.what);
	auto size = is.tellg();
	is.seekg(0);
	auto url = "http://" + host + "/books";
	curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)size);
	buffer.clear();
	post(url);
	is.close();
	read_book();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CBook b)
{
	auto url = "http://" + host + "/books/" + std::to_string(b.id);
	get(url);
	read_book();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CPage p)
{
	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(p.id);
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CSetBook& sb)
{
	auto url = "http://" + host + "/books/" + std::to_string(bookid);
	auto content = sb.key + "=" + sb.val;
	assert(curl);
	CurlStr str(curl_easy_escape(curl, content.data(), (int)content.size()));
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(str.get()));
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.get());
	post(url);
	read_book();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CChangeFile& cf)
{
	std::string line;
	is.open(cf.file);
	if (not is.good())
		throw std::system_error(errno, std::system_category(), cf.file);
	std::getline(is, line);
	is.close();
	(*this)(CChangeStr{line});
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CChangeStr& cs)
{
	THROW(NotImplemented, "Not implemented");
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CNextPage np)
{
	if (np.ofs < 0)
		return (*this)(CPrevPage{std::abs(np.ofs)});
	if (pageid == 0 or np.ofs == 0)
		return (*this)(CFirstPage{});

	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(pageid) + "/next/" +
		std::to_string(np.ofs);
	buffer.clear();
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CPrevPage pp)
{
	if (pp.ofs < 0)
		return (*this)(CNextPage{std::abs(pp.ofs)});
	if (pageid == 0 or pp.ofs == 0)
		return (*this)(CLastPage{});

	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(pageid) + "/prev/" +
		std::to_string(pp.ofs);
	buffer.clear();
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CFirstPage)
{
	auto url = "http://" + host + "/books/" +
		std::to_string(bookid) + "/pages/first";
	buffer.clear();
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CLastPage)
{
	auto url = "http://" + host + "/books/" +
		std::to_string(bookid) + "/pages/last";
	buffer.clear();
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CNextLine nl)
{
	if (nl.ofs < 0)
		return (*this)(CPrevLine{std::abs(nl.ofs)});
	if (pageid == 0) {
		(*this)(CFirstPage{});
		(*this)(CFirstLine{});
	}
	if (lineid + nl.ofs >= static_cast<int>(page.size())) {
		(*this)(CNextPage{1});
		(*this)(CFirstLine{});
	} else if (nl.ofs == 0) {
		(*this)(CFirstLine{});
	} else {
		lineid += nl.ofs;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CPrevLine pl)
{
	if (pl.ofs < 0)
		return (*this)(CNextLine{std::abs(pl.ofs)});
	if (pageid == 0) {
		(*this)(CLastPage{});
		(*this)(CLastLine{});
	}
	if (lineid < pl.ofs) {
		(*this)(CPrevPage{1});
		(*this)(CLastLine{});
	} else if (pl.ofs == 0) {
		(*this)(CLastLine{});
	} else {
		lineid -= pl.ofs;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CFirstLine)
{
	lineid = 0;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CLastLine)
{
	lineid = page.empty() ? 0 : static_cast<int>(page.size()) - 1;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CPrint& p) const
{
	auto print = [this](const CPrint& p, int id) {
		if (p.cor) {
			std::cout << page[id].cor << "\n";
		} else {
			std::cout << page[id].ocr << "\n";
		}
	};
	if (p.b == 0 and p.e == 0) {
		print(p, lineid);
	} else {
		const auto end = std::min(p.e, static_cast<int>(page.size()));
		const auto begin = std::min(p.b, static_cast<int>(page.size()));
		for (auto i = begin; i != end; ++i) {
			std::cout << i << ":";
			print(p, i);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CQuit)
{
	done = true;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CInfo) const
{
	std::cerr << "tmp dir: " << tmpdir.dir() << "\n";
	std::cerr << "host:    " << host << "\n";
	std::cerr << "port:    " << port << "\n";
	std::cerr << "book id: " << bookid << "\n";
	std::cerr << "page id: " << pageid << "\n";
	std::cerr << "pages:   " << npages << "\n";
	std::cerr << "line id: " << lineid << "\n";
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CError& error) const
{
	std::cerr << "[error] " << error.what << "\n";
}

////////////////////////////////////////////////////////////////////////////////
Command
Ed::parse(const std::string& line)
{
	static const std::regex versionre{R"(:vers?i?o?n?\s*)"};
	static const std::regex quitre{R"(:qu?i?t?\s*)"};
	static const std::regex booksre{R"(:books\s*)"};
	static const std::regex bookre{R"(:book\s+(\d+)\s*)"};
	static const std::regex pagere{R"(:page\s+(\d+)\s*)"};
	static const std::regex uploadre{R"(:upl?o?a?d?\s+(.+))"};
	static const std::regex nextpagere{R"(:ne?x?t?pa?g?e?\s*([-+]?\d+)?)"};
	static const std::regex prevpagere{R"(:pr?e?v?pa?g?e?\s*([-+]?\d+)?)"};
	static const std::regex nextlinere{R"(:ne?x?t?l?i?n?e?\s*([-+]?\d+)?)"};
	static const std::regex prevlinere{R"(:pr?e?v?l?i?n?e?\s*([-+]?\d+)?)"};
	static const std::regex infore{R"(:info\s*)"};
	static const std::regex printre{R"(((\d+),(\d+))?:pri?n?t(ocr)?\s*)"};
	static const std::regex changefilere{R"(:ch?a?n?g?e?\s+file:\s*(.*))"};
	static const std::regex changestrre{R"(:ch?a?n?g?e?\s+(.*))"};
	static const std::regex setbookre{R"(:se?t?\s+([^=]+)\s*=\s*(.+)\s*)"};

	std::smatch m;
	if (std::regex_match(line, versionre))
		return CVersion{};
	if (std::regex_match(line, quitre))
		return CQuit{};
	if (std::regex_match(line, booksre))
		return CBooks{};
	if (std::regex_match(line, m, bookre))
		return CBook{std::stoi(m[1])};
	if (std::regex_match(line, m, pagere))
		return CPage{std::stoi(m[1])};
	if (std::regex_match(line, m, uploadre))
		return CUpload{m[1]};
	if (std::regex_match(line, m, setbookre))
		return CSetBook{m[1], m[2]};
	if (std::regex_match(line, m, nextpagere)) {
		if (m[1].length())
			return CNextPage{std::stoi(m[1])};
		else
			return CNextPage{1};
	}
	if (std::regex_match(line, m, nextlinere)) {
		if (m[1].length())
			return CNextLine{std::stoi(m[1])};
		else
			return CNextLine{1};
	}
	if (std::regex_match(line, m, prevpagere)) {
		if (m[1].length())
			return CPrevPage{std::stoi(m[1])};
		else
			return CPrevPage{1};
	}
	if (std::regex_match(line, m, prevlinere)) {
		if (m[1].length())
			return CPrevLine{std::stoi(m[1])};
		else
			return CPrevLine{1};
	}
	if (std::regex_match(line, m, printre)) {
		const bool cor = m[4].length();
		if (m[1].length())
			return CPrint{std::stoi(m[2]), std::stoi(m[3]), cor};
		else
			return CPrint{0, 0, cor};
	}
	if (std::regex_match(line, m, changefilere)) { // must check this first
		return CChangeFile{m[1]};
	}
	if (std::regex_match(line, m, changestrre)) {
		return CChangeStr{m[1]};
	}
	if (std::regex_match(line, infore))
		return CInfo{};
	else
		return CError{"Invalid command: " + line};

}

////////////////////////////////////////////////////////////////////////////////
void
Ed::read_book()
{
	auto json = crow::json::load(buffer);
	bookid = static_cast<int>(json["id"]);
	npages = static_cast<int>(json["pages"]);
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::read_page()
{
	auto json = crow::json::load(buffer);
	pageid = static_cast<int>(json["id"]);
	page.clear();
	for (auto& line: json["lines"]) {
		page.emplace_back(line["cor"].s(), line["ocr"].s());
	}
}

////////////////////////////////////////////////////////////////////////////////
size_t
xread(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto is = reinterpret_cast<std::ifstream*>(userdata);
	is->read(static_cast<char*>(ptr), size * nmemb);
	auto res = is->gcount();
	return res;
}

////////////////////////////////////////////////////////////////////////////////
size_t
xwrite(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	reinterpret_cast<std::string*>(userdata)->append(ptr, size *nmemb);
	return size * nmemb;
}

////////////////////////////////////////////////////////////////////////////////
static int
run(int argc, char* argv[])
{
	Ed ed{argc, argv};
	ed.login();
	std::string line;
	while (not ed.done and std::getline(std::cin, line)) {
		ed(line);
	}
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try {
		std::locale::global(std::locale(""));
		return run(argc, argv);
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	} catch (...) {
		std::cerr << "[error] ??\n";
	}
	return EXIT_FAILURE;
}

