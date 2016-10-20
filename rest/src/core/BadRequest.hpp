#ifndef pcw_BadRequest_hpp__
#define pcw_BadRequest_hpp__

#include <stdexcept>

namespace pcw {
	class BadRequest: public std::runtime_error {
	public:
		BadRequest(const char *what): std::runtime_error(what) {}
		BadRequest(const std::string& what): std::runtime_error(what) {}
		virtual ~BadRequest() noexcept override = default;
	};
}

#endif // pcw_BadRequest_hpp__
