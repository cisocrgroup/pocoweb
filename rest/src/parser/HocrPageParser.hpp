#ifndef pcw_HocrPageParser_hpp__
#define pcw_HocrPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "core/PageParser.hpp"
#include "core/XmlFile.hpp"

namespace pcw {
	class Box;

	class HocrPageParser: public PageParser, 
			      public XmlFile,
			      public pugi::xml_tree_walker {
	public:
		HocrPageParser(const Path& path);
		virtual ~HocrPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return page_node_;}
		virtual PagePtr parse() override;
		virtual bool begin(XmlNode& node) override;
		virtual bool for_each(XmlNode& node) override;

	private:
		void next_page();

		static double get_conf(const XmlNode& node);
		static std::string get_img(const XmlNode& node);
		static Box get_box(const XmlNode& node);

		XmlNode page_node_;
		PagePtr page_;
	};
}

#endif // pcw_HocrPageParser_hpp__
