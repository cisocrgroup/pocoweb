#include "SuspiciousWordsRoute.hpp"
#include <crow/app.h>
#include <chrono>
#include "core/App.hpp"
#include "core/Book.hpp"
#include "core/Config.hpp"
#include "core/Session.hpp"
#include "core/jsonify.hpp"
#include "core/util.hpp"
#include "database/Tables.h"

#define SUSPICIOUS_WORDS_ROUTE_ROUTE_1 "/books/<int>/suspicious-words"
#define SUSPICIOUS_WORDS_ROUTE_ROUTE_2 "/books/<int>/pages/<int>/suspicious-words"
#define SUSPICIOUS_WORDS_ROUTE_ROUTE_3 \
	"/books/<int>/pages/<int>/lines/<int>/suspicious-words"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* SuspiciousWordsRoute::route_ =
    SUSPICIOUS_WORDS_ROUTE_ROUTE_1 ","
    SUSPICIOUS_WORDS_ROUTE_ROUTE_2 ","
    SUSPICIOUS_WORDS_ROUTE_ROUTE_3;
const char* SuspiciousWordsRoute::name_ = "SuspiciousWordsRoute";

////////////////////////////////////////////////////////////////////////////////
void SuspiciousWordsRoute::Register(App& app) {
	CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_1).methods("GET"_method)(*this);
	CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_2).methods("GET"_method)(*this);
	CROW_ROUTE(app, SUSPICIOUS_WORDS_ROUTE_ROUTE_3).methods("GET"_method)(*this);
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response SuspiciousWordsRoute::impl(HttpGet, const Request& req,
						  int bid) const {
	return internal_server_error();
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response SuspiciousWordsRoute::impl(HttpGet, const Request& req,
						  int bid, int pid) const {
	return internal_server_error();
}

////////////////////////////////////////////////////////////////////////////////
SuspiciousWordsRoute::Response SuspiciousWordsRoute::impl(HttpGet, const Request& req,
						  int bid, int pid, int lid) const {
	return internal_server_error();
}

