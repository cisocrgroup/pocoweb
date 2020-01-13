#ifndef pcw_Error_hpp__
#define pcw_Error_hpp__

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pcw {
class Error : public std::exception {
public:
  Error(const char *f, int l, const char *what)
      : what_(std::string(f) + ":" + std::to_string(l) + ": " + what + ": ") {}
  Error(const char *f, int l)
      : what_(std::string(f) + ":" + std::to_string(l) + ": ") {}
  virtual ~Error() noexcept override = default;
  virtual const char *what() const noexcept override { return what_.data(); }
  virtual int code() const noexcept {
    // InternalServerError
    return 500;
  }
  virtual const char *status() const noexcept {
    return "Internal Server Error";
  }

  template <class T> Error &append(const T &t) {
    std::stringstream os;
    os << t;
    what_ += os.str();
    return *this;
  }

private:
  std::string what_;
};

class BadRequest : public Error {
public:
  BadRequest(const char *f, int l) : Error(f, l, "BadRequest") {}
  virtual ~BadRequest() noexcept override = default;
  virtual int code() const noexcept override {
    // BadRequest
    return 400;
  }
  virtual const char *status() const noexcept override { return "Bad Request"; }
};

class Conflict : public Error {
public:
  Conflict(const char *f, int l) : Error(f, l, "Conflict") {}
  virtual ~Conflict() noexcept override = default;
  virtual int code() const noexcept override {
    // Conflict
    return 409;
  }
  virtual const char *status() const noexcept override { return "Conflict"; }
};

class NotImplemented : public Error {
public:
  NotImplemented(const char *f, int l) : Error(f, l, "NotImplemented") {}
  virtual ~NotImplemented() noexcept override = default;
  virtual int code() const noexcept override {
    // NotImplemented
    return 501;
  }
  virtual const char *status() const noexcept override {
    return "Not Implemented";
  }
};

class ServiceNotAvailable : public Error {
public:
  ServiceNotAvailable(const char *f, int l)
      : Error(f, l, "ServiceNotAvailable") {}
  virtual ~ServiceNotAvailable() noexcept override = default;
  virtual int code() const noexcept override {
    // ServiceNotAvailable
    return 503;
  }
  virtual const char *status() const noexcept override {
    return "Service Not Available";
  }
};

class Forbidden : public Error {
public:
  Forbidden(const char *f, int l) : Error(f, l, "Forbidden") {}
  virtual ~Forbidden() noexcept override = default;
  virtual int code() const noexcept override {
    // Forbidden
    return 403;
  }
  virtual const char *status() const noexcept override { return "Forbidden"; }
};

class NotFound : public Error {
public:
  NotFound(const char *f, int l) : Error(f, l, "NotFound") {}
  virtual ~NotFound() noexcept override = default;
  virtual int code() const noexcept override {
    // NotFound
    return 404;
  }
  virtual const char *status() const noexcept override { return "Not Found"; }
};

class ParseError : public Error {
public:
  ParseError(const char *f, int l) : Error(f, l, "ParseError") {}
  virtual ~ParseError() noexcept override = default;
  virtual int code() const noexcept override {
    // ParseError (BadRequest)
    return 400;
  }
  virtual const char *status() const noexcept override { return "Parse Error"; }
};

template <class E> void do_append(E &e) {}

template <class E, class T, class... Args>
void do_append(E &e, const T &t, Args &&... args) {
  e.append(t);
  do_append(e, std::forward<Args>(args)...);
}

template <class E>[[noreturn]] void do_throw(const char *f, int l) {
  throw E(f, l);
}

template <class E, class... Args>
[[noreturn]] void do_throw(const char *f, int l, Args &&... args) {
  E e(f, l);
  do_append(e, std::forward<Args>(args)...);
  throw e;
}
} // namespace pcw

#define THROW(e, args...) pcw::do_throw<e>(__FILE__, __LINE__, ##args)

#endif // pcw_BadRequest_hpp__
