#ifndef pcw_Route_hpp__
#define pcw_Route_hpp__

#include <memory>
#include <vector>

namespace crow {
	template<typename... Middleware> class Crow;
	struct response;
}

namespace pcw {
	class Route;
	using RoutePtr = std::unique_ptr<Route>;

	class Route {
	public:
		using App = crow::Crow<>;
		virtual ~Route() noexcept = default;
		virtual void Register(App&) = 0;
		virtual const char* route() const noexcept = 0;
		virtual const char* name() const noexcept = 0;
		virtual void Deregister(App&) {}
	};

	class Routes: private std::vector<RoutePtr> {
	public:
		using Base = std::vector<RoutePtr>;
		using Base::value_type;
		using Base::push_back;
		using Base::begin;
		using Base::end;
	};
}

#endif // pcw_Route_hpp__
