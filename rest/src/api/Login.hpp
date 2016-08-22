#ifndef api_Login_hpp__
#define api_Login_hpp__

#include "util/hash.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableBooks.hpp"
#include "doc/BookData.hpp"
#include "doc/Book.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "Config.hpp"
#include "Api.hpp"

namespace pcw {
	
	template<class S> class Login: public Api<S, Login<S>> {
	public:
		using Base = typename pcw::Api<S, Login<S>>::Base;
		using Server = typename Base::Server;
		using Status = typename Base::Status;
		using Content = typename Base::Content;

		void do_reg(Server& server) const noexcept;
		Status run(Content& content) const noexcept;
	};
}
////////////////////////////////////////////////////////////////////////////////
template<class S>
void 
pcw::Login<S>::do_reg(Server& server) const noexcept 
{
	static const char *uri = "^/login/username/([^/]+)/password/([^/]+)$";
	server.server.resource[uri]["GET"] = *this;
	server.server.resource[uri]["PUSH"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::Login<S>::Status
pcw::Login<S>::run(Content& content) const noexcept
{
	const auto username = content.req->path_match[1];
	const auto password = content.req->path_match[2];
	auto conn = connect(this->config());
	const auto user = User::create(*conn, username);
	
	if (not user or not user->authenticate(*conn, password))
		return Status::Forbidden;
	do {
		content.sid = gensessionid(16);
	} while (not this->sessions().insert(content.sid.get(), user));
	
	using json = nlohmann::json;
	json j;
	j["api"] = PCW_API_VERSION;
	j["user"] = user->json();
	
	DbTableBooks books{conn};
	const auto allowedBooks = books.getAllowedBooks(user->id);
	for (const auto& book: allowedBooks) {
		json jj;
		book->store(jj);
		j["books"].push_back(jj);
	}
	BOOST_LOG_TRIVIAL(info) << *user << ": " << content.sid.get() << " logged on";
	content.os << j;
	return Status::Ok;
}

#endif // api_Login_hpp__
