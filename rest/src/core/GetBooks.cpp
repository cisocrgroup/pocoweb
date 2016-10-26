#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "BadRequest.hpp"
#include "User.hpp"
#include "Page.hpp"
#include "Book.hpp"
#include "Database.hpp"
#include "GetBooks.hpp"
#include "Sessions.hpp"
#include "ScopeGuard.hpp"
#include "BookDir.hpp"

#define GET_BOOKS_ROUTE_1 "/books/<int>"
#define GET_BOOKS_ROUTE_2 "/books/<int>/pages"
#define GET_BOOKS_ROUTE_3 "/books/<int>/pages/<int>"
#define GET_BOOKS_ROUTE_4 "/books/<int>/pages/<int>/lines"
#define GET_BOOKS_ROUTE_5 "/books/<int>/pages/<int>/lines/<int>"


using namespace pcw;

static crow::json::wvalue book_to_json(const Book& book);
static crow::json::wvalue page_to_json(const Page& page);
static crow::json::wvalue line_to_json(const Line& line);
static crow::json::wvalue box_to_json(const Box& box);

////////////////////////////////////////////////////////////////////////////////
const char* GetBooks::route_ = 
	GET_BOOKS_ROUTE_1 ","
	GET_BOOKS_ROUTE_2 ","
	GET_BOOKS_ROUTE_3 ","
	GET_BOOKS_ROUTE_4 ","
	GET_BOOKS_ROUTE_5;
const char* GetBooks::name_ = "GetBooks";

////////////////////////////////////////////////////////////////////////////////
void
GetBooks::Register(App& app)
{
	CROW_ROUTE(app, GET_BOOKS_ROUTE_1).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_3).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_4).methods("GET"_method)(*this);
	CROW_ROUTE(app, GET_BOOKS_ROUTE_5).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int bookid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	auto book = std::dynamic_pointer_cast<Book>(db->session().current_project);
	if (not book or bookid != book->id) {
		std::lock_guard<std::mutex> lock(db->session().mutex);
		book = db->select_book(bookid);
		db->session().current_project = book;
	}
	// missing authentication
	return book_to_json(*book);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int bookid, int pageid) const
{
	auto db = this->database(req);
	if (not db)
		return forbidden();
	auto book = std::dynamic_pointer_cast<Book>(db->session().current_project);
	if (not book or bookid != book->id) {
		std::lock_guard<std::mutex> lock(db->session().mutex);
		book = db->select_book(bookid);
		db->session().current_project = book;
	}
	// missing authentication
	return page_to_json(*(*book)[pageid - 1]);
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int bookid, int pageid, int lineid) const
{
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue 
book_to_json(const Book& book)
{
	crow::json::wvalue j;
	j["id"] = book.id;
	j["uri"] = book.uri;
	j["author"] = book.author;
	j["title"] = book.title;
	j["year"] = book.year;
	j["description"] = book.description;
	j["pages"] = book.size();
	
	std::vector<int> ids;
	ids.resize(book.size());
	std::transform(begin(book), end(book), begin(ids), [](const auto& p) {
		assert(p);
		return p->id;
	});
	j["pageIds"] = ids;
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue 
page_to_json(const Page& page)
{
	crow::json::wvalue j;
	j["id"] = page.id;
	j["box"] = box_to_json(page.box);
	j["ocrFile"] = page.ocr.native();
	j["imgFile"] = page.img.native();
	
	//std::vector<crow::json::wvalue> lines;
	//lines.reserve(page.size());
	//std::transform(begin(page), end(page), begin(lines), line_to_json);	
	size_t i = 0;
	for (const auto& line: page)
		j["lines"][i++] = line_to_json(line);
	// j["lines"] = lines;
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue 
line_to_json(const Line& line)
{
	crow::json::wvalue j;
	j["id"] = line.id;
	j["box"] = box_to_json(line.box);
	j["imgFile"] = line.img.native();
	j["str"] = line.string();
	j["cuts"] = line.cuts();
	j["confidences"] = line.confidences();
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue 
box_to_json(const Box& box)
{
	crow::json::wvalue j;
	j["left"] = box.left();
	j["right"] = box.right();
	j["top"] = box.top();
	j["bottom"] = box.bottom();
	j["width"] = box.width();
	j["height"] = box.height();
	return j;
}
