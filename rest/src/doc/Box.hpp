#ifndef pcw_Box_hpp__
#define pcw_Box_hpp__

#include "json.hpp"

namespace pcw {
	struct Box {
		Box(int a = 0, int b = 0, int c = 0, int d = 0)
			: left(a)
			, top(b)
			, right(c)
			, bottom(d)
		{}
		Box(nlohmann::json& json): Box() {load(json);}
		int width() const noexcept {return right - left;}
		int height() const noexcept {return bottom - top;}
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;

		int left, top, right, bottom;
	};
}

#endif // pcw_Box_hpp__
