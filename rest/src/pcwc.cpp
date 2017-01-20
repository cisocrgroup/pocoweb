#include <boost/variant.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <regex>
#include <curl/curl.h>
#include <crow/json.h>
#include "utils/Error.hpp"
#include "utils/TmpDir.hpp"

using namespace pcw;

struct CurlFree {
	void operator()(char* str) const noexcept {
		curl_free(str);
	}
};
using CurlStr = std::unique_ptr<char, CurlFree>;

static const std::string RANGE_RE = R"(((\d*)(,(\d*))?))";
struct Range {
	Range(const std::smatch& m)
		: b(0)
		, e(0)
	{
		if (m[1].length()) {
			if (m[2].length())
				b = std::stoi(m[2]) - 1;
			else
				b = 0;
			if (m[4].length())
				e = std::stoi(m[4]) - 1;
			else
				e = std::numeric_limits<int>::max();
		}
		if (b < 0 or e < 0)
			THROW(Error, "Invalid range: ", b, ", ", e);
	}
	bool current_line() const noexcept {
		return b == 0 and e == 0;
	}
	int b, e;
};
enum struct What {Book, Page, Line};

struct CVersion {};
struct CQuit {};
struct CBooks {};
struct CGoto {What what; int id;};
struct CRaw {std::string str; bool get;};
struct CNextPage {int ofs;};
struct CPrevPage {int ofs;};
struct CSetBook {std::map<std::string, std::string> data;};
struct CChange {std::string what; bool is_file;};
struct CFirstPage {};
struct CLastPage {};
struct CFirstLine {};
struct CLastLine {};
struct CNextLine {int ofs;};
struct CPrevLine {int ofs;};
struct CPrint {Range r; bool cor;};
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
	CGoto,
	CRaw,
	CSetBook,
	CChange,
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

	void operator()(const std::string& line);
	void operator()(CVersion);
	void operator()(CQuit);
	void operator()(CBooks);
	void operator()(const CSetBook& set);
	void operator()(const CChange& c);
	void operator()(const CUpload& c);
	void operator()(CGoto g);
	void operator()(const CRaw& r);
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

	void goto_book(int id);
	void goto_page(int id);
	void goto_line(int id);
	void change(const std::string& line);
	void read_book();
	void read_page();
	void read_line();

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
	if (argc < 5)
		THROW(Error, "Usage: ", argv[0], " <host> <port> <user> <pass> [commands...]");
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
	case 202:
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
			  << book["year"] << " "
			  << book["description"] << " "
			  << book["language"] << "\n";
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
Ed::operator()(CGoto g)
{
	switch (g.what) {
	case What::Page:
		goto_page(g.id);
		break;
	case What::Book:
		goto_book(g.id);
		break;
	case What::Line:
		goto_line(g.id);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::goto_book(int id)
{
	auto url = "http://" + host + "/books/" + std::to_string(id);
	get(url);
	read_book();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::goto_page(int id)
{
	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(id);
	get(url);
	read_page();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::goto_line(int id)
{
	--id;
	if (id < 0 or id >= static_cast<int>(page.size()))
		THROW(Error, "Invalid line index: ", id);
	lineid = id;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CRaw& r)
{
	auto url = "http://" + host + "/" + r.str;
	if (r.get)
		get(url);
	else
		post(url);
	std::cout << buffer << "\n";
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CSetBook& sb)
{
	assert(curl);
	auto url = "http://" + host + "/books/" + std::to_string(bookid) + "?";

	for (const auto& p: sb.data) {
		CurlStr key(curl_easy_escape(curl, p.first.data(), p.first.size()));
		CurlStr val(curl_easy_escape(curl, p.second.data(), p.second.size()));
		// std::cerr << "Setting key: " << key.get() << "\n";
		// std::cerr << "Setting val: " << val.get() << "\n";
		url += key.get();
		url += "=";
		url += val.get();
		url += "&";
	}
	url.pop_back(); // remove trailing '&' or '?'
	// std::cerr << "url: " << url << "\n";
	post(url);
	read_book();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(const CChange& c)
{
	if (c.is_file) {
		std::string line;
		is.open(c.what);
		if (not is.good())
			throw std::system_error(errno, std::system_category(), c.what);
		std::getline(is, line);
		is.close();
		change(line);
	} else {
		change(c.what);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::change(const std::string& str)
{
	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(pageid) +
		"/lines/" + std::to_string(lineid + 1) +
		"?partial=false&correction=";
	assert(curl);
	CurlStr params(curl_easy_escape(curl, str.data(), str.size()));
	url += params.get();
	post(url);
	read_line();
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
	if (p.r.current_line()) {
		std::cout << lineid + 1 << ":";
		print(p, lineid);
	} else {
		const auto end = std::min(p.r.e, static_cast<int>(page.size()));
		const auto begin = std::min(p.r.b, static_cast<int>(page.size()));
		for (auto i = begin; i < end; ++i) {
			std::cout << i + 1 << ":";
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
	std::cerr << "line id: " << lineid + 1 << "\n";
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
	static const std::regex linere{R"(:line\s+(\d+)\s*)"};
	static const std::regex uploadre{R"(:upl?o?a?d?\s+(.+))"};
	static const std::regex nextpagere{R"(:ne?x?t?pa?g?e?\s*([-+]?\d+)?)"};
	static const std::regex prevpagere{R"(:pr?e?v?pa?g?e?\s*([-+]?\d+)?)"};
	static const std::regex nextlinere{R"(:ne?x?t?l?i?n?e?\s*([-+]?\d+)?)"};
	static const std::regex prevlinere{R"(:pr?e?v?l?i?n?e?\s*([-+]?\d+)?)"};
	static const std::regex infore{R"(:info\s*)"};
	static const std::regex printre{RANGE_RE + R"(:pri?n?t?(ocr)?\s*)"};
	static const std::regex changefilere{R"(:ch?a?n?g?e?\s+file:\s*(.*))"};
	static const std::regex changestrre{R"(:ch?a?n?g?e?\s+(.*))"};
	static const std::regex setbookre{R"(:se?t?\s+((\s*([^=]+)\s*=\s*([^;]+);)+))"};
	static const std::regex keyvalre{R"(\s*([^=]+)\s*=\s*([^;]+);)"};
	static const std::regex rawre{R"(:raw\s+(get|post)\s+(.*))"};

	std::smatch m;
	if (std::regex_match(line, versionre))
		return CVersion{};
	if (std::regex_match(line, quitre))
		return CQuit{};
	if (std::regex_match(line, booksre))
		return CBooks{};
	if (std::regex_match(line, m, bookre))
		return CGoto{What::Book, std::stoi(m[1])};
	if (std::regex_match(line, m, pagere))
		return CGoto{What::Page, std::stoi(m[1])};
	if (std::regex_match(line, m, linere))
		return CGoto{What::Line, std::stoi(m[1])};
	if (std::regex_match(line, m, uploadre))
		return CUpload{m[1]};
	if (std::regex_match(line, m, setbookre)) {
		const auto e = end(line);
		CSetBook keyvals;
		for (auto i = m[1].first; i != e;) {
			if (std::regex_search(i, e, m, keyvalre)) {
				keyvals.data[m[1]] = m[2];
				i = m[0].second;
			} else {
				i = e;
			}
		}
		return keyvals;
	}
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
		const bool cor = not m[5].length();
		return CPrint{Range(m), cor};
	}
	if (std::regex_match(line, m, changefilere)) { // must check this first
		return CChange{m[1], true};
	}
	if (std::regex_match(line, m, changestrre)) {
		return CChange{m[1], false};
	}
	if (std::regex_match(line, m, rawre)) {
		return CRaw{m[2], m[1] == "get"};
	}
	if (std::regex_match(line, infore))
		return CInfo{};
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
void
Ed::read_line()
{
	auto json = crow::json::load(buffer);
	lineid = static_cast<int>(json["id"]) - 1;

	if (lineid < 0)
		THROW(Error, "Invalid line id from server: ", lineid);
	page[lineid].cor = json["cor"].s();
	page[lineid].ocr = json["ocr"].s();
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
static void
prompt(bool p, std::chrono::duration<double> time = {})
{
	if (p) {
		if (time.count() > 0)
			std::cout << "[finished in " << time.count() << "s]\n";
		std::cout << "? " << std::flush;
	}
}

////////////////////////////////////////////////////////////////////////////////
static int
run(int argc, char* argv[])
{
	bool p = isatty(fileno(stdin));
	Ed ed{argc, argv};
	ed.login();

	for (int i = 5; i < argc; ++i) {
		ed(argv[i]);
	}

	std::string line;
	if (not ed.done)
		prompt(p);
	while (not ed.done and std::getline(std::cin, line)) {
		auto start = std::chrono::system_clock::now();
		ed(line);
		auto end = std::chrono::system_clock::now();
		prompt(p, end - start);
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

