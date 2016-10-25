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

static crow::json::wvalue& add_book(crow::json::wvalue& j, const Book& book);
static crow::json::wvalue& add_page(crow::json::wvalue& j, const Page& page);
static crow::json::wvalue& add_line(crow::json::wvalue& j, const Line& line);
static crow::json::wvalue& add_box(crow::json::wvalue& j, const Box& box);

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
	auto session = this->session(req);
	if (not session)
		return forbidden();
	auto book = session->current_book;
	if (not book or book->owner->id != session->user->id)
		return not_implemented();

	crow::json::wvalue j;
	add_book(j, *book);
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int bookid, int pageid) const
{
	auto session = this->session(req);
	if (not session)
		return forbidden();
	auto book = session->current_book;
	if (not book or book->owner->id != session->user->id)
		return not_implemented();

	crow::json::wvalue j;
	return add_page(j, *(*book)[pageid - 1]);
	add_book(j, *book);
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::response
GetBooks::operator()(const crow::request& req, int bookid, int pageid, int lineid) const
{
	return not_implemented();
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue& 
add_book(crow::json::wvalue& j, const Book& book)
{
	j["id"] = book.id;
	j["uri"] = book.uri;
	j["author"] = book.author;
	j["title"] = book.title;
	j["year"] = book.year;
	j["description"] = book.description;
	j["pages"] = book.size();
	
	std::vector<int> ids;
	std::transform(begin(book), end(book), std::back_inserter(ids), [](const auto& p) {
		assert(p);
		return p->id;
	});
	j["pageIds"] = ids;
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue& 
add_page(crow::json::wvalue& j, const Page& page)
{
	j["id"] = page.id;
	add_box(j["box"], page.box);
	j["ocrFile"] = page.ocr.native();
	j["imgFile"] = page.img.native();
	
	crow::json::wvalue lines(crow::json::type::List);
	for (const auto& line: page)
		add_line(lines[line.id - 1], line);
	j["lines"] = std::move(lines);
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue& 
add_line(crow::json::wvalue& j, const Line& line)
{
	j["id"] = line.id;
	add_box(j["box"], line.box);
	j["imgFile"] = line.img.native();
	j["str"] = line.string();
	j["cuts"] = line.cuts();
	j["confidences"] = line.confidences();
	return j;
}

////////////////////////////////////////////////////////////////////////////////
crow::json::wvalue& 
add_box(crow::json::wvalue& j, const Box& box)
{
	j["left"] = box.left();
	j["right"] = box.right();
	j["top"] = box.top();
	j["bottom"] = box.bottom();
	j["width"] = box.width();
	j["height"] = box.height();
	return j;
}
