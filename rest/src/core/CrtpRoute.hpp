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
	} catch (const std::logic_error& e) {
		CROW_LOG_ERROR << e.what();
		return internal_server_error();
	} catch (const std::runtime_error& e) {
		CROW_LOG_ERROR << e.what();
		return bad_request();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << e.what();
		return internal_server_error();
	} catch (...) {
		CROW_LOG_ERROR << "catch (...)";
		return internal_server_error();
	}
}

#endif // pcw_CrtpRoute_hpp__
