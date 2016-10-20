#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "Book.hpp"
#include "Database.hpp"
#include "CreateBook.hpp"
#include "Sessions.hpp"

#define CREATE_BOOK_ROUTE "/create/book/author/<string>/title/<string>"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* CreateBook::route_ = CREATE_BOOK_ROUTE;
const char* CreateBook::name_ = "CreateBook";

////////////////////////////////////////////////////////////////////////////////
void
CreateBook::Register(App& app)
{
	CROW_ROUTE(app, CREATE_BOOK_ROUTE).methods("POST"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
CreateBook::operator()(
	const crow::request& request,
	const std::string& author,
	const std::string& title
) const {
	auto db = database(request);
	if (not db)
		return forbidden();

	std::lock_guard<std::mutex> lock(db->session().mutex);
	db->set_autocommit(false);

	auto book = db->insert_book(author, title);
	if (not book) // should not happen
		return internal_server_error();
	try {
		CROW_LOG_INFO << "(CreateBook) BookDir: " << book->directory.path();
		book->directory.add(raw_content(request));
	} catch (const BookDirError& e) {
		CROW_LOG_ERROR << "(CreateBook) Error: " << e.what();
		book->directory.remove();
		return bad_request();
	}
		
	db->update_book_pages(*book);
	db->session().current_book = book;
	db->commit();
	return ok();
}

////////////////////////////////////////////////////////////////////////////////
std::string 
CreateBook::raw_content(const crow::request& request)
{
	static const std::string ContentType{"Content-Type"};
	static const std::regex BoundaryRegex{R"(boundary=(.*);?$)"};
	CROW_LOG_INFO << "### BODY ###\n" << request.body;

	std::smatch m;
	if (std::regex_search(request.get_header_value(ContentType), m, BoundaryRegex))
		return multipart(request, m[1]);
	else 
		return raw(request);
}

////////////////////////////////////////////////////////////////////////////////
std::string 
CreateBook::multipart(const crow::request& request, const std::string& boundary)
{
	CROW_LOG_INFO << "(CreateBook) Boundary in multipart data: " << boundary;
	auto b = request.body.find("\r\n\r\n");
	if (b == std::string::npos)
		throw BookDirError("(CreateBook) Missing '\\r\\n\\r\\n' in multipart data");
	b += 4;
	auto e = request.body.rfind(boundary);
	if (b == std::string::npos)
		throw BookDirError("(CreateBook) Boundary in multipart data" + boundary);
	if (e < b)
		throw BookDirError("(CreateBook) Invalid Boundary in multipart data" + boundary);
	CROW_LOG_INFO << "(CreateBook) b = " << b << ", e = " << e;
	
	return request.body.substr(b, e - b);
}

////////////////////////////////////////////////////////////////////////////////
std::string 
CreateBook::raw(const crow::request& request)
{
	return request.body;
}

