#ifndef pcw_Cache_hpp__
#define pcw_Cache_hpp__

#include <memory>
#include <mutex>
#include <vector>

namespace pcw {
	template<class T>
	class Cache: private std::vector<std::shared_ptr<T>> {
	public:
		using Base = std::vector<std::shared_ptr<T>>;
		using value_type = typename Base::value_type;

		Cache(size_t n): n_(n) {this->reserve(n);}
		using Base::begin;
		using Base::end;
		using Base::size;
		using Base::empty;
		
		template<class G>
		value_type get(int id, G g) noexcept;
		template<class G>
		value_type get(const std::string& name, G g) noexcept;

	private:
		template<class F, class G>
		value_type do_get(F f, G g) noexcept;
		std::mutex mutex_;
		const size_t n_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::get(int id, G g) noexcept
{
	auto gg = [id,g](){return g(id);};
	auto f = [id](const value_type& t) {return t ? t->id() == id : false;};
	return do_get(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::get(const std::string& name, G g) noexcept
{
	auto gg = [&name,g](){return g(name);};
	auto f = [&name](const value_type& t) {return t ? t->name == name : false;};
	return do_get(f, gg);
}

////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class F, class G>
typename pcw::Cache<T>::value_type 
pcw::Cache<T>::do_get(F f, G g) noexcept
{
	std::lock_guard<std::mutex> lock(mutex_);
	const auto b = begin();
	const auto e = end();
	auto i = std::find_if(b, e, f);

	if (i == e) {
		auto t = g();
		if (not t)
			return nullptr;
		if (size() < n_)
			this->push_back(t);
		else
			this->back() = t;
		return this->back();
	} else {
		if (i != b) {
			auto j = std::prev(i);
			std::swap(*i, *j);
			return *j;
		} else {
			return this->front();
		}
	}
}

#endif // pcw_Cache_hpp__
