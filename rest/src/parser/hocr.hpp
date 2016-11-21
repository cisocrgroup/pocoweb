#ifndef pcw_hocr_hpp__
#define pcw_hocr_hpp__

namespace pugi {
	class xml_node;
	class xml_document;
}

namespace pcw {
	class Box;
	class Xml;

	namespace hocr {
		bool is(const pugi::xml_document& doc);

		Box get_box(pugi::xml_node node);
		std::wstring get_cont(pugi::xml_node node);
		double get_conf(pugi::xml_node node);
		std::string get_img(pugi::xml_node node);

		void set_box(const Box& box, pugi::xml_node node);
		void set_cont(const std::wstring& cont, pugi::xml_node node);
		void set_conf(double conf, pugi::xml_node node);
		void set_img(const std::string& img, pugi::xml_node node);
	}
}

#endif // pcw_hocr_hpp__
