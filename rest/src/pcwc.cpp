#include <boost/variant.hpp>
#include <iostream>
#include <regex>
#include "core/Error.hpp"
#include "core/TmpDir.hpp"

using namespace pcw;

struct Address {
	int b, e;
};

struct CVersion {};
struct CQuit {};
struct CError {std::string line;};

using Command = boost::variant<
	CVersion,
	CQuit,
	CError
>;

struct Data {
	Data(char* argv[])
		: host(argv[1])
		, user(argv[3])
		, pass(argv[4])
		, line()
		, port(std::stoi(argv[2]))
		, tmpdir()
	{}
	std::string host, user, pass, line;
	int port;
	TmpDir tmpdir;
};

static Command parse_command(const std::string& line);
static Command parse_command(const char* line);
static Data parse_args(int argc, char* argv[]);
static int run(int argc, char* argv[]);

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

////////////////////////////////////////////////////////////////////////////////
int
run(int argc, char* argv[])
{
	auto data = parse_args(argc, argv);
	while (std::getline(std::cin, data.line)) {
		auto command = parse_command(data.line);
		if (boost::get<CVersion>(&command)) {
			std::cout << "VERSION\n";
		} else if (boost::get<CQuit>(&command)) {
			break;
		} else {
			std::cout << "[error] Invalid line: "
				  << boost::get<CError>(command).line << "\n";
		}
	}
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
Command
parse_command(const std::string& line)
{
	return parse_command(line.data());
}

////////////////////////////////////////////////////////////////////////////////
Command
parse_command(const char* line)
{
	static const std::regex versionre{R"(:vers?i?o?n?\s*)"};
	static const std::regex quitre{R"(:qu?i?t?\s*)"};
	if (std::regex_match(line, versionre))
		return CVersion{};
	if (std::regex_match(line, quitre))
		return CQuit{};
	else
		return CError{line};
}

////////////////////////////////////////////////////////////////////////////////
Data
parse_args(int argc, char* argv[])
{
	if (argc != 5)
		THROW(Error, "Usage: ", argv[0], " <host> <port> <user> <pass>");
	if (std::stoi(argv[2]) <= 0)
		THROW(Error, "Expceted port > 0, got: ", argv[2]);
	return Data{argv};
}
