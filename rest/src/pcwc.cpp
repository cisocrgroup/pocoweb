#include <boost/variant.hpp>
#include <iostream>
#include <regex>
#include <curl/curl.h>
#include "core/Error.hpp"
#include "core/TmpDir.hpp"

using namespace pcw;

struct Address {
	int b, e;
};

struct CVersion {};
struct CQuit {};
struct CError {std::string what;};

using Command = boost::variant<
	CVersion,
	CQuit,
	CError
>;

static char EBUF[CURL_ERROR_SIZE + 1];
static size_t xwrite(char *ptr, size_t size, size_t nmemb, void *userdata);

struct Ed: boost::static_visitor<void> {
	Ed(int argc, char* argv[]);
	~Ed() noexcept;

	void login();
	void operator()(const std::string& line);
	void operator()(CVersion) const;
	void operator()(CQuit);
	void operator()(const CError& error) const;

	static Command parse(const std::string& line);

	std::string host, user, pass;
	TmpDir tmpdir;
	CURL* curl;
	int port;
	bool done;
};

////////////////////////////////////////////////////////////////////////////////
Ed::Ed(int argc, char* argv[])
	: host()
	, user()
	, pass()
	, tmpdir()
	, curl()
	, port()
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
	curl = curl_easy_init();
	if (not curl)
		THROW(Error, "Could not initialize curl");
	auto url = "http://" + host + "/login/username/" + user + "/password/" + pass;
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, EBUF);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &xwrite);
	curl_easy_setopt(curl, CURLOPT_PORT, port);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	if (curl_easy_perform(curl) != CURLE_OK)
		THROW(Error, "curl_easy_perform(): ", EBUF);
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
Ed::operator()(CVersion) const
{
	std::cout << "VERSION\n";
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
	if (std::regex_match(line, versionre))
		return CVersion{};
	if (std::regex_match(line, quitre))
		return CQuit{};
	else
		return CError{"Invalid command: " + line};

}

////////////////////////////////////////////////////////////////////////////////
size_t
xwrite(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::cout.write(ptr, size * nmemb);
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

