#ifndef pcw_Cache_hpp__
#define pcw_Cache_hpp__

#include <algorithm>
#include <cassert>
#include <list>
#include <memory>
#include <mutex>

namespace pcw {
template <class T>
class Cache : private std::list<std::shared_ptr<T>> {
       public:
	using Base = std::list<std::shared_ptr<T>>;
	using value_type = typename Base::value_type;

	Cache(size_t n) : n_(n) {}
	using Base::begin;
	using Base::end;
	using Base::size;
	using Base::empty;

	size_t size() const noexcept {
		Lock lock(mutex_);
		return Base::size();
	}
	size_t max_size() const noexcept { return n_; }
	void put(value_type t);
	template <class G>
	value_type get(int id, G g);
	template <class G>
	value_type get(const std::string& name, G g);

       private:
	using Iterator = typename Base::iterator;
	using Mutex = std::mutex;
	using Lock = std::lock_guard<Mutex>;

	void put_impl(value_type t);
	template <class F, class G>
	value_type get_impl(F f, G g);

	mutable Mutex mutex_;
	const size_t n_;
};
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
void pcw::Cache<T>::put(value_type t) {
	Lock lock(mutex_);
	put_impl(std::move(t));
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
void pcw::Cache<T>::put_impl(value_type t) {
	assert(not mutex_.try_lock());
	// never put nullptr
	if (t) {
		// new elements are inserted at the back of the cache
		if (Base::size() < n_) {
			Base::push_back(std::move(t));
		} else {
			Base::back() = std::move(t);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
template <class G>
typename pcw::Cache<T>::value_type pcw::Cache<T>::get(int id, G g) {
	Lock lock(mutex_);
	auto gg = [id, g]() { return g(id); };
	auto f = [id](const value_type& t) { return t->id() == id; };
	return get_impl(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
template <class G>
typename pcw::Cache<T>::value_type pcw::Cache<T>::get(const std::string& name,
						      G g) {
	Lock lock(mutex_);
	auto gg = [&name, g]() { return g(name); };
	auto f = [&name](const value_type& t) { return t->name == name; };
	return get_impl(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
template <class F, class G>
typename pcw::Cache<T>::value_type pcw::Cache<T>::get_impl(F f, G g) {
	assert(not mutex_.try_lock());
	auto i = std::find_if(begin(), end(), f);
	if (i == end()) {
		auto t = g();
		put_impl(t);
		return t;
	} else if (i != begin()) {
		auto j = std::prev(i);
		std::swap(*i, *j);
		return *j;
	} else {
		return *i;
	}
}

#endif  // pcw_Cache_hpp__
