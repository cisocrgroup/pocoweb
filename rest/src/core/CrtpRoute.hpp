#ifndef pcw_CrtpRoute_hpp__
#define pcw_CrtpRoute_hpp__

#include <crow/logging.h>
#include "core/Route.hpp"

namespace pcw {
template <class T>
class CrtpRoute : public Route {
       public:
	struct HttpGet {};
	struct HttpPost {};
	struct HttpPut {};
	struct HttpDelete {};

	virtual ~CrtpRoute() noexcept override = default;

	template <class... Args>
	Response operator()(const Request& req, Args&&... args) const noexcept;

       private:
	const T& that() const noexcept { return static_cast<const T&>(*this); }
};
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
template <class... Args>
pcw::Route::Response pcw::CrtpRoute<T>::operator()(const Request& req,
						   Args&&... args) const
    noexcept {
	try {
		for (const auto& kv : req.headers) {
			CROW_LOG_DEBUG << "(CrtpRoute) Request: " << kv.first
				       << "=" << kv.second;
		}
		Response res;
		switch (req.method) {
			case crow::HTTPMethod::Get:
				res = that().impl(HttpGet{}, req,
						  std::forward<Args>(args)...);
				break;
			case crow::HTTPMethod::Post:
				CROW_LOG_DEBUG << "(CrtpRoute) POST BODY: "
					       << req.body;
				res = that().impl(HttpPost{}, req,
						  std::forward<Args>(args)...);
				break;
			case crow::HTTPMethod::Put:
				res = that().impl(HttpPut{}, req,
						  std::forward<Args>(args)...);
				break;
			case crow::HTTPMethod::Delete:
				res = that().impl(HttpDelete{}, req,
						  std::forward<Args>(args)...);
				break;
			default:
				return not_found();
		}
		for (const auto& kv : res.headers) {
			CROW_LOG_DEBUG << "(CrtpRoute) Response: " << kv.first
				       << "=" << kv.second;
		}
		CROW_LOG_DEBUG << "(CrtpRoute) Response BODY: " << res.body;
		return res;
	} catch (const Error& e) {
		CROW_LOG_ERROR << e.what();
		return Response(e.code());
	} catch (const std::system_error& e) {
		CROW_LOG_ERROR << e.what();
		return internal_server_error();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << e.what();
		return internal_server_error();
	} catch (...) {
		CROW_LOG_ERROR << "catch (...)";
		return internal_server_error();
	}
}

#define pcw_crtp_route_def_impl__(args...)                                  \
	template <class T>                                                  \
	pcw::Route::Response impl(T, const Request& req, ##args) const {    \
		pcw::do_throw<pcw::NotImplemented>(                         \
		    __FILE__, __LINE__, "Not implemented: ", req.url, ": ", \
		    typeid(T).name());                                      \
		return pcw::Route::internal_server_error();                 \
	}

#endif  // pcw_CrtpRoute_hpp__
