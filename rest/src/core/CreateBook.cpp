#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "BadRequest.hpp"
#include "Page.hpp"
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
	BookPtr book = nullptr;
	try {
		auto db = database(request);
		if (not db)
			return forbidden();

		std::lock_guard<std::mutex> lock(db->session().mutex);
		db->set_autocommit(false);
		book = db->insert_book(author, title);
		if (not book) // should not happen
			return internal_server_error();

		CROW_LOG_INFO << "(CreateBook) BookDir: " << book->directory.path();
		book->directory.setup(extract_content(request), *book);
		book->directory.clean_up();
		db->update_book_pages(*book);
		db->session().current_book = book;
		db->commit();
		for (const auto& page: *book) {
			for (const auto& line: *page) {
				CROW_LOG_INFO << "(CreateBook) Line: " << line.string();
			}
		}
		return ok();
	} catch (const BadRequest& e) {
		CROW_LOG_ERROR << "(CreateBook) Error: " << e.what();
		if (book) 
			book->directory.remove();
		return bad_request();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(CreateBook) Error: " << e.what();
		if (book)
			book->directory.remove();
		return internal_server_error();
	}
}
