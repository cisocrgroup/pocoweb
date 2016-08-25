#include <boost/log/trivial.hpp>
#include <mysql_connection.h>
#include "server_http.hpp"
#include "db/db.hpp"
#include "db/Sessions.hpp"
#include "Config.hpp"
#include "Login.hpp"
#include "Post.hpp"
#include "api.hpp"
#include "Api.hpp"

////////////////////////////////////////////////////////////////////////////////
template<class S> class Default: public pcw::Api<S, Default<S>> {
public:
	using Base = typename pcw::Api<S, Default<S>>::Base;
	using Server = typename Base::Server;
	using Status = typename Base::Status;
	using Content = typename Base::Content;

	void do_reg(Server& server) const noexcept {
		server.server.default_resource["POST"] = *this;
		server.server.default_resource["PUT"] = *this;
		server.server.default_resource["GET"] = *this;
	}
	Status run(const Content& content) const noexcept {
		BOOST_LOG_TRIVIAL(debug) << "(Default) BadRequest: " 
					<< content.req->path_match[0];
		return Status::BadRequest;
	}
};

////////////////////////////////////////////////////////////////////////////////
void
pcw::run(std::shared_ptr<Config> config)
{
	assert(config);
	using S = SimpleWeb::Server<SimpleWeb::HTTP>;
	Server<S> server(config);
	
	Login<Server<S>> login;
	PostBook<Server<S>> pbook;
	PostPageImage<Server<S>> ppimg;
	PostPageXml<Server<S>> ppxml;
	Default<Server<S>> def;
	
	login.reg(server);
	pbook.reg(server);
	ppimg.reg(server);
	ppxml.reg(server);
	def.reg(server);

	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] starting ";
	std::thread sthread([&server](){server.start();});
	sthread.join();
	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] stopping";
}
