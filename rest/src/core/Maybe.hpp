#ifndef pcw_Maybe_hpp__
#define pcw_Maybe_hpp__

#include <boost/variant.hpp>

//
// inspired (copied) from: https://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
//
namespace pcw {
	template<class T> class Maybe {
	public:
		Maybe(const T& tt): t(tt), error_(false) {}
		Maybe(T&& tt): t(std::move(tt)), error_(false) {}
		Maybe(const Maybe<T>& o): error_(o.error_) {
			if (not error_)
				new(&t) T(o.t);
			else
				new(&e) std::exception_ptr(o.e);
		}
		Maybe(Maybe<T>&& o): error_(o.error_) {
			if (not error_)
				new(&t) T(std::move(o.t));
			else
				new(&e) std::exception_ptr(std::move(o.e));
		}
		~Maybe() noexcept {
			using std::exception_ptr;
			if (not error_)
				t.~T();
			else
				e.~exception_ptr();
		}
		void swap(Maybe<T>& o) {
			if (not error_) {
				if (not o.error_) {
					using std::swap;
					swap(t, o.t);
				} else {
					auto tmp = std::move(o.e);
					new(&o.t) T(std::move(t));
					new(&e) std::exception_ptr(tmp);
					std::swap(error_, o.error_);
				}
			} else {
				if (not o.error_) {
					o.swap(*this);
				} else {
					e.swap(o.e);
					std::swap(error_, o.error_);
				}
			}
		}
		template<class E>
		static Maybe<T> from_exception(const E& e) {
			if (typeid(e) != typeid(E)) {
				throw std::invalid_argument("slicing detected");
			}
			return from_exception(std::make_exception_ptr(e));
		}
		static Maybe<T> from_exception(std::exception_ptr e) noexcept {
			Maybe<T> maybe;
			maybe.error_ = true;
			new(&maybe.e) std::exception_ptr(std::move(e));
			return maybe;
		}
		static Maybe<T> from_exception() noexcept {
			return from_exception(std::current_exception());
		}
		template<class F>
		static Maybe<T> from_lambda(F f) {
			try {
				return Maybe<T>(f());
			} catch (...) {
				return from_exception();
			}
		}

		T& get() {
			if (error_)
				std::rethrow_exception(e);
			return t;
		}
		const T& get() const {
			if (error_)
				std::rethrow_exception(e);
			return t;
		}
		bool ok() const noexcept {
			return not error_;
		}
		template<class E>
		bool has() const noexcept {
			try {
				if (error_)
					std::rethrow_exception(e);
			} catch (const E&) {
				return true;
			} catch (...) {
			}
			return false;
		}

		template<class E>
		const char *what() const noexcept {
			try {
				if (error_)
					std::rethrow_exception(e);
			} catch (const E&e ) {
				return e.what();
			} catch (...) {
			}
			return nullptr;
		}

	private:
		Maybe() {}
		union {
			T t;
			std::exception_ptr e;
		};
		bool error_;
	};
}

#endif // pcw_Maybe_hpp__
