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

	private:
		SessionPtr make_session(Content& content) const noexcept;
		void write_response(const Session& session, Content& content) const;
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
	// make session (nullptr means invalid user)
	auto session = make_session(content);
	if (not session)
		return Status::Forbidden;

	// write result
	write_response(*session, content);
	return Status::Ok;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
pcw::SessionPtr 
pcw::Login<S>::make_session(Content& content) const noexcept
{
	const auto username = content.req->path_match[1];
	const auto password = content.req->path_match[2];
	const auto connection = connect(this->config());
	const auto user = User::create(*connection, username);
	if (not user or not user->authenticate(*connection, password))
		return nullptr;

	while (true) { // must create unique session
		auto sid = gensessionid(16);
		auto session = this->sessions().new_session(sid);
		if (session) {
			session->user = user;
			session->connection = connection;
			session->sid = sid;
			content.sid = sid;
			return session;
		}
	}
	assert(false);
	return nullptr;
}
	
////////////////////////////////////////////////////////////////////////////////
template<class S>
void
pcw::Login<S>::write_response(const Session& session, Content& content) const
{
	using json = nlohmann::json;
	json j;
	j["api"] = PCW_API_VERSION;
	j["user"] = session.user->json();
	
	DbTableBooks books{session.connection};
	const auto allowedBooks = books.getAllowedBooks(session.user->id);
	for (const auto& book: allowedBooks) {
		json jj;
		book->store(jj);
		j["books"].push_back(jj);
	}
	BOOST_LOG_TRIVIAL(info) << *session.user 
				<< ": " << session.sid << " logged on";
	content.os << j;
}

#endif // api_Login_hpp__
