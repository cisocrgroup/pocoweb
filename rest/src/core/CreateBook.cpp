#include <cppconn/connection.h>
#include <crow.h>
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
	CROW_ROUTE(app, CREATE_BOOK_ROUTE)(*this);
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
	auto book = db->insert_book(author, title);
	if (not book) // should not happen
		return internal_server_error();
	db->session().current_book = book;
	return ok();
}
