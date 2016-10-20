#ifndef pcw_CreateBook_hpp__
#define pcw_CreateBook_hpp__

#include "Route.hpp"

namespace pcw {
	class CreateBook: public Route {
	public:
		virtual ~CreateBook() noexcept override = default;
		virtual void Register(App& app) override;
		virtual const char* route() const noexcept override {return route_;}
		virtual const char* name() const noexcept override {return name_;}
		crow::response operator()(
			const crow::request& request,
			const std::string& author,
			const std::string& title
		) const;

	private:
		static std::string raw_content(const crow::request& request);
		static std::string multipart(const crow::request& request, const std::string& boundary);
		static std::string raw(const crow::request& request);
	

		static const char* route_;
		static const char* name_;
	};
}

#endif // pcw_CreateBook_hpp__
