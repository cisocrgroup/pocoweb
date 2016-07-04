#ifndef pcw_Box_hpp__
#define pcw_Box_hpp__

namespace pcw {
	struct Box {
		Box(int a = 0, int b = 0, int c = 0, int d = 0)
			: x0(a)
			, y0(b)
			, x1(c)
			, y1(d)
		{}
		Box(nlohmann::json& json): Box() {load(json);}
		int x0, y0, x1, y1;
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
	};
}

#endif // pcw_Box_hpp__
