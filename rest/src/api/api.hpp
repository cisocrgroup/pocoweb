#ifndef pcw_api_hpp__
#define pcw_api_hpp__

namespace pcw {
	struct Api {
		using Server = SimpleWeb::Server<SimpleWeb::HTTP>;
		using RequestPtr = std::shared_ptr<Server::Request>;
		using Response = Server::Response;

		void ok(Response& response, const std::string& content) const noexcept;
		void badRequest(Response& response, const std::string& content) const noexcept;
		void internalServerError(Response& response) const noexcept;
	};

	struct Config;
	struct Login: public Api {
		Login(const Config& c): config(c) {}
		void operator()(Response& response, RequestPtr request) const noexcept;

		const Config& config;
	};
}

#endif // pcw_api_hpp__
