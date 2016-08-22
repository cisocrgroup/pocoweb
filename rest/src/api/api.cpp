#include <boost/log/trivial.hpp>
#include "server_http.hpp"
#include "db/Sessions.hpp"
#include "Config.hpp"
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
		server.server.default_resource["GET"] = *this;
	}
	Status run(const Content&) const noexcept {
		return Status::BadRequest;
	}
};

////////////////////////////////////////////////////////////////////////////////
void
pcw::run(std::shared_ptr<Config> config)
{
	// const auto config = std::make_shared<Config>(Config::load(cfile));
	assert(config);
	using S = SimpleWeb::Server<SimpleWeb::HTTP>;
	Server<S> server(config);
	
	Default<Server<S>> def;
	def.reg(server);

	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] starting ";
	std::thread sthread([&server](){server.start();});
	sthread.join();
	BOOST_LOG_TRIVIAL(info) << "daemon[" << getpid() << "] stopping";
}
