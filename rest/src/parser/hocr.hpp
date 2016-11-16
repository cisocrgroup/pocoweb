#ifndef pcw_hocr_hpp__
#define pcw_hocr_hpp__

namespace pugi {
	class xml_node;
}

namespace pcw {
	class Box;
	namespace hocr {
		Box get_box(pugi::xml_node node);
		const char* get_cont(pugi::xml_node node);
		double get_conf(pugi::xml_node node);
		std::string get_img(pugi::xml_node node);
	}
}

#endif // pcw_hocr_hpp__
