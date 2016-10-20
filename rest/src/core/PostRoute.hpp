#ifndef pcw_PostRoute_hpp__
#define pcw_PostRoute_hpp__

#include "Route.hpp"

namespace pcw {
	class PostRoute: public Route {
	public:
		virtual ~PostRoute() noexcept override = default;

	protected:
		static std::string extract_content(const crow::request& request);

	private:
		static std::string extract_multipart(
			const crow::request& request,
			const std::string& boundary
		);
		static std::string extract_raw(const crow::request& request);
	};
}

#endif // pcw_PostRoute_hpp__
