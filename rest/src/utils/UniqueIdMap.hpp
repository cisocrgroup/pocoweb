#ifndef pcw_UniqueIdMap_hpp__
#define pcw_UniqueIdMap_hpp__

#include <map>

namespace pcw {
template <class T>
class UniqueIdMap {
       public:
	std::pair<int, bool> operator[](const T& t) {
		auto i = map_.find(t);
		if (i == end(map_)) {
			const auto newid = static_cast<int>(map_.size()) + 1;
			map_.emplace_hint(i, t, newid);
			return {newid, true};
		} else {
			return {i->second, false};
		}
	}

       private:
	std::map<T, int> map_;
};
}
#endif  // pcw_UniqueIdMap_hpp__
