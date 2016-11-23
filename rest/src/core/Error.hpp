#ifndef pcw_Error_hpp__
#define pcw_Error_hpp__

#include <stdexcept>

namespace pcw {
	class Error: public std::runtime_error {
	public:
		Error(const char *what): std::runtime_error(what) {}
		Error(const std::string& what): std::runtime_error(what) {}
		virtual ~Error() noexcept override = default;
	};
	class BadRequest: public Error {
	public:
		BadRequest(const char *what): Error(what) {}
		BadRequest(const std::string& what): Error(what) {}
		virtual ~BadRequest() noexcept override = default;
	};
}

#endif // pcw_BadRequest_hpp__
