#ifndef pcw_CrtpRoute_hpp__
#define pcw_CrtpRoute_hpp__

#include "core/Route.hpp"

namespace pcw {
	template<class T>
	class CrtpRoute: public Route {
	public:
		struct HttpGet {};
		struct HttpPost {};
		struct HttpPut {};
		struct HttpDelete {};

		virtual ~CrtpRoute() noexcept override = default;

		template<class... Args>
		Response operator()(const Request& req, Args&&... args) const noexcept;

	private:
		const T& that() const noexcept {
			return static_cast<const T&>(*this);
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class... Args>
pcw::Route::Response
pcw::CrtpRoute<T>::operator()(const Request& req, Args&&... args) const noexcept
{
	try {
		for (const auto& kv: req.headers) {
			CROW_LOG_INFO << "(CrtpRoute) "
				      << kv.first << "=" << kv.second;
		}
		switch (req.method) {
		case crow::HTTPMethod::Get:
			return that().impl(HttpGet{}, req, std::forward<Args>(args)...);
		case crow::HTTPMethod::Post:
			return that().impl(HttpPost{}, req, std::forward<Args>(args)...);
		case crow::HTTPMethod::Put:
			return that().impl(HttpPut{}, req, std::forward<Args>(args)...);
		default:
			return not_found();
		}
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

#define pcw_crtp_route_def_impl__(args...) \
	template<class T> \
	pcw::Route::Response \
	impl(T, const Request& req, ##args) const { \
		pcw::do_throw<pcw::NotImplemented>( \
			__FILE__, __LINE__, \
			"Not implemented: ", req.url, ": ", \
			typeid(T).name() \
		); \
		return pcw::Route::internal_server_error(); \
	}

#endif // pcw_CrtpRoute_hpp__
