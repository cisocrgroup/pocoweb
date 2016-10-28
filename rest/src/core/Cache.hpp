#ifndef pcw_Cache_hpp__
#define pcw_Cache_hpp__

#include <memory>
#include <mutex>
#include <list>

namespace pcw {
	template<class T>
	class Cache: private std::list<std::shared_ptr<T>> {
	public:
		using Base = std::list<std::shared_ptr<T>>;
		using value_type = typename Base::value_type;

		Cache(size_t n): n_(n) {}
		using Base::begin;
		using Base::end;
		using Base::size;
		using Base::empty;
		
		void put(value_type t);
		template<class G>
		value_type get(int id, G g);
		template<class G>
		value_type get(const std::string& name, G g);

	private:
		using Iterator = typename Base::iterator;
		template<class F, class G>
		value_type do_get(F f, G g);
		template<class G>
		value_type update(Iterator i, G g);
		std::mutex mutex_;
		const size_t n_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
void
pcw::Cache<T>::put(value_type t)
{
	std::lock_guard<std::mutex> lock(mutex_);
	// new elements are inserted at the back of the cache
	if (Base::size() < n_) {
		Base::push_back(std::move(t));
	} else {
		Base::back() = std::move(t);
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::get(int id, G g)
{
	auto gg = [id,g](){return g(id);};
	auto f = [id](const value_type& t) {return t->id() == id;};
	return do_get(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::get(const std::string& name, G g)
{
	auto gg = [&name,g](){return g(name);};
	auto f = [&name](const value_type& t) {return t->name == name;};
	return do_get(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class F, class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::do_get(F f, G g)
{
	auto i = end();
	// just lock the cache while searching
	// update() locks the cache again if it needs to add an element
	// this makes it save to recursively use the cache from g()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		i = std::find_if(begin(), end(), f);
	}
	return update(i, g);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::update(Iterator i, G g)
{
	const auto b = begin();
	const auto e = end();

	if (i == e) {
		auto t = g();
		if (not t) {
			return nullptr;
		} else {
			std::lock_guard<std::mutex> lock(mutex_);
			if (size() < n_)
				this->push_back(t);
			else
				this->back() = t;
			return this->back();
		}
	} else {
		std::lock_guard<std::mutex> lock(mutex_);
		if (i != b) {
			auto j = std::prev(i);
			std::swap(*i, *j);
			return *j;
		} else {
			return *i;
		}
	}
}

#endif // pcw_Cache_hpp__
