#ifndef pcw_Box_hpp__
#define pcw_Box_hpp__

namespace nlohman {
	class json;
}

namespace pcw {
	struct Box {
		int x0, y0, x1, y1;
		void store(nlohmann::json& json) const;
	};
}

#endif // pcw_Box_hpp__
