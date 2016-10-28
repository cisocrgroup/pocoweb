#include <cppconn/connection.h>
#include <regex>
#include <crow.h>
#include "BadRequest.hpp"
#include "Page.hpp"
#include "Book.hpp"
#include "Database.hpp"
#include "CreateBook.hpp"
#include "Sessions.hpp"
#include "ScopeGuard.hpp"
#include "BookDir.hpp"

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
	try {
		// session
		auto db = database(request);
		if (not db)
			return forbidden();

		// create new bookdir
		BookDir dir(config()); 
		ScopeGuard sg([&dir](){dir.remove();});
		CROW_LOG_INFO << "(CreateBook) BookDir: " << dir.dir();
		dir.add_zip_file(extract_content(request));
		auto book = dir.build();
		if (not book)
			return internal_server_error();
		book->author = author;
		book->title = title;
		book->set_owner(*db->session().user);
		
		// insert book into database
		CROW_LOG_INFO << "(CreateBook) Inserting new book into database";
		std::lock_guard<std::mutex> lock(db->session().mutex);
		db->set_autocommit(false);
		db->insert_book(*book);
		db->commit();

		// update and clean up
		CROW_LOG_INFO << "(CreateBook) Created new book id: " << book->id();
		sg.dismiss();
		return created();
	} catch (const BadRequest& e) {
		CROW_LOG_ERROR << "(CreateBook) Error: " << e.what();
		return bad_request();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "(CreateBook) Error: " << e.what();
		return internal_server_error();
	}
}
