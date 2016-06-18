#ifndef pcw_api_hpp__
#define pcw_api_hpp__

namespace pcw {
	class User;
	struct Config;
	class Sessions;
	using SessionsPtr = std::shared_ptr<Sessions>;

	struct Api {
		using Server = SimpleWeb::Server<SimpleWeb::HTTP>;
		using RequestPtr = std::shared_ptr<Server::Request>;
		using Response = Server::Response;
		enum class Status {Ok, BadRequest, InternalServerError, Forbidden};

		Api(): Api(nullptr) {}
		Api(SessionsPtr s): sessions(std::move(s)) {}
		void ok(Response& response, const std::string& content) const noexcept;
		void badRequest(Response& response, const std::string& content) const noexcept;
		void internalServerError(Response& response) const noexcept;
		void forbidden(Response& response) const noexcept;
		void reply(Status s, Response& r, const std::string& content) const noexcept;
		static std::unique_ptr<Server> server(const Config& config);

		const SessionsPtr sessions;
	};

	/*
	 * call: .../rest-api/login?username=...&password=password
	 * json: {
	 * 	"api-version": "0.1.0",
	 *	"sessionid": "session-id",
	 *	"user": {
	 *		"name": "username",
	 *		"email": "email",
	 *		"institute": "Institute"
	 *	}
	 * }
         */
	struct Login: public Api {
		Login(SessionsPtr s, const Config& config)
			: Api(std::move(s))
			, config_(config)
		{}
		void operator()(Response& response, RequestPtr request) const noexcept;

	private:
		Status doLogin(const std::string& username,
			       const std::string& password,
			       std::string& answer) const;
		void createSessionAndWrite(User& user, std::string& answer) const;

		const Config& config_;
	};

	/*
	 * call: .../rest-api/getPage?sessionid=...&pageid=...&bookid=...
         * json: {
	 *         ...
	 */
	struct GetPage: public Api {
		GetPage(SessionsPtr s, const Config& config)
			: Api(std::move(s))
			, config_(config)
		{}
		void operator()(Response& response, RequestPtr request) const noexcept;

	private:
		Status doGetPage(const std::string& sid,
				 int bookid, int pageid, std::string& answer) const;
		const Config& config_;
	};
}

#endif // pcw_api_hpp__
