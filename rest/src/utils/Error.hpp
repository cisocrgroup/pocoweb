#ifndef pcw_Error_hpp__
#define pcw_Error_hpp__

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pcw {
	class Error: public std::exception {
	public:
		Error(const char* f, int l, const char* what)
			: what_(std::string(f) + ":" + std::to_string(l) + ": " + what + ": ")
		{}
		Error(const char* f, int l)
			: what_(std::string(f) + ":" + std::to_string(l) + ": ")
		{}
		virtual ~Error() noexcept override = default;
		virtual const char* what() const noexcept override {
			return what_.data();
		}
		virtual int code() const noexcept {
			// InternalServerError
			return 500;
		}

		template<class T>
		Error& append(const T& t) {
			std::stringstream os;
			os << t;
			what_ += os.str();
			return *this;
		}

	private:
		std::string what_;
	};

	class BadRequest: public Error {
	public:
		BadRequest(const char* f, int l): Error(f, l, "BadRequest") {}
		virtual ~BadRequest() noexcept override = default;
		virtual int code() const noexcept override {
			// BadRequest
			return 400;
		}
	};

	class NotImplemented: public Error {
	public:
		NotImplemented(const char* f, int l): Error(f, l, "NotImplemented") {}
		virtual ~NotImplemented() noexcept override = default;
		virtual int code() const noexcept override {
			// NotImplemented
			return 501;
		}
	};
	class Forbidden: public Error {
	public:
		Forbidden(const char* f, int l): Error(f, l, "Forbidden") {}
		virtual ~Forbidden() noexcept override = default;
		virtual int code() const noexcept override {
			// Forbidden
			return 403;
		}
	};

	class NotFound: public Error {
	public:
		NotFound(const char* f, int l): Error(f, l, "NotFound") {}
		virtual ~NotFound() noexcept override = default;
		virtual int code() const noexcept override {
			// NotFound
			return 404;
		}
	};

	class ParseError: public Error {
	public:
		ParseError(const char* f, int l): Error(f, l, "ParseError") {}
		virtual ~ParseError() noexcept override = default;
		virtual int code() const noexcept override {
			// ParseError (BadRequest)
			return 400;
		}
	};

	template<class E>
	void do_append(E& e) {}

	template<class E, class T, class... Args>
	void do_append(E& e, const T& t, Args&&... args) {
		e.append(t);
		do_append(e, std::forward<Args>(args)...);
	}

	template<class E>
	[[noreturn]] void do_throw(const char *f, int l) {
		throw E(f, l);
	}

	template<class E, class... Args>
	[[noreturn]] void do_throw(const char *f, int l, Args&&... args) {
		E e(f, l);
		do_append(e, std::forward<Args>(args)...);
		throw e;
	}
}

#define THROW(e, args...) pcw::do_throw<e>(__FILE__, __LINE__, ##args)

#endif // pcw_BadRequest_hpp__