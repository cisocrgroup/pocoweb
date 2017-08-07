#ifndef pcw_UniqueIdMap_hpp__
#define pcw_UniqueIdMap_hpp__

#include <map>

namespace pcw {
template <class T>
class UniqueIdMap : private std::map<T, int> {
       private:
	using base = std::map<T, int>;

       public:
	std::pair<int, bool> operator[](const T& t) {
		auto i = base::find(t);
		auto isnew = false;
		if (i == base::end()) {
			auto newid = static_cast<int>(base::size()) + 1;
			i = base::emplace_hint(i, t, newid);
			isnew = true;
		}
		return {i->second, isnew};
	}
};
}
#endif  // pcw_UniqueIdMap_hpp__
