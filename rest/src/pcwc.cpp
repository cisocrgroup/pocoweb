#include <boost/variant.hpp>
#include <iostream>
#include <fstream>
#include <regex>
#include <curl/curl.h>
#include <crow/json.h>
#include "core/Error.hpp"
#include "core/TmpDir.hpp"

using namespace pcw;

struct Address {
	int b, e;
};

struct CVersion {};
struct CQuit {};
struct CBooks {};
struct CBook {int id;};
struct CNextPage{int ofs;};
struct CUpload {std::string what;};
struct CError {std::string what;};

using Command = boost::variant<
	CVersion,
	CBooks,
	CUpload,
	CBook,
	CNextPage,
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
	void get(const std::string& url) const;
	void post(const std::string& url) const;

	void operator()(const std::string& line);
	void operator()(CVersion);
	void operator()(CQuit);
	void operator()(CBooks);
	void operator()(const CUpload& c);
	void operator()(CBook b);
	void operator()(CNextPage np);
	void operator()(const CError& error) const;

	static Command parse(const std::string& line);

	std::string host, user, pass, buffer;
	std::ifstream is;
	TmpDir tmpdir;
	Path cookiefile;
	std::vector<std::string> page;
	CURL* curl;
	int port, bookid, pageid, lineid;
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
Ed::get(const std::string& url) const
{
	assert(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_PUT, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 0);
	perform();
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::post(const std::string& url) const
{
	assert(curl);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
	curl_easy_setopt(curl, CURLOPT_PUT, 0);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
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
	auto json = crow::json::load(buffer);
	bookid = static_cast<int>(json["id"]);
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CBook b)
{
	bookid = b.id;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CNextPage np)
{
	auto url = "http://" + host + "/books/" + std::to_string(bookid) +
		"/pages/" + std::to_string(pageid) + "/" + std::to_string(np.ofs);
	buffer.clear();
	get(url);
	auto json = crow::json::load(buffer);
	std::cout << json;
}

////////////////////////////////////////////////////////////////////////////////
void
Ed::operator()(CQuit)
{
	done = true;
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
	static const std::regex uploadre{R"(:upl?o?a?d?\s+(.+))"};
	static const std::regex nextpagere{R"(:ne?x?t?pa?g?e?\s*([-+]?\d+)?)"};

	std::smatch m;
	if (std::regex_match(line, versionre))
		return CVersion{};
	if (std::regex_match(line, quitre))
		return CQuit{};
	if (std::regex_match(line, booksre))
		return CBooks{};
	if (std::regex_match(line, m, uploadre))
		return CUpload{m[1]};
	if (std::regex_match(line, m, uploadre)) {
		if (m[1].length())
			return CNextPage{std::stoi(m[1])};
		else
			return CNextPage{1};
	}
	else
		return CError{"Invalid command: " + line};

}

////////////////////////////////////////////////////////////////////////////////
size_t
xread(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto is = reinterpret_cast<std::ifstream*>(userdata);
	is->read(static_cast<char*>(ptr), size * nmemb);
	auto res = is->gcount();
	// std::cerr << "xread(ptr, " << size << ", " << nmemb << ", data): " << res << "\n";
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
		return run(argc, argv);
	} catch (const std::exception& e) {
		std::cerr << "[error] " << e.what() << "\n";
	} catch (...) {
		std::cerr << "[error] ??\n";
	}
	return EXIT_FAILURE;
}

