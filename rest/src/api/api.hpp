#ifndef pcw_api_hpp__
#define pcw_api_hpp__

namespace pcw {
	struct Config;

	struct Api {
		using Server = SimpleWeb::Server<SimpleWeb::HTTP>;
		using RequestPtr = std::shared_ptr<Server::Request>;
		using Response = Server::Response;
		enum class Status {Ok, BadRequest, InternalServerError, Forbidden};

		void ok(Response& response, const std::string& content) const noexcept;
		void badRequest(Response& response, const std::string& content) const noexcept;
		void internalServerError(Response& response) const noexcept;
		void forbidden(Response& response) const noexcept;
		void reply(Status s, Response& r, const std::string& content) const noexcept;
		static std::unique_ptr<Server> server(const Config& config);
	};

	struct Login: public Api {
		Login(const Config& c): config(c) {}
		void operator()(Response& response, RequestPtr request) const noexcept;

	private:
		Status doLogin(const std::string& username,
			       const std::string& password,
			       std::string& answer) const noexcept;
		const Config& config;
	};
}

#endif // pcw_api_hpp__
