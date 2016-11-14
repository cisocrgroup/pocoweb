#ifndef pcw_XmlParserPage_hpp__
#define pcw_XmlParserPage_hpp__

#include <memory>
#include "BasicParserPage.hpp"
#include "Xml.hpp"

namespace pcw {
	class XmlParserPage: public BasicParserPage {
	public:
		using Node = pugi::xml_node;
		using Doc = pugi::xml_document;
		using DocPtr = std::shared_ptr<Doc>;

		XmlParserPage(Xml xml = {}): xml_(xml) {}
		XmlParserPage(const Path& path): xml_(path) {}
		virtual ~XmlParserPage() noexcept override = default;
		virtual void write(const Path& path) const override;
		Xml& xml() noexcept {return xml_;}
		const Xml& xml() const noexcept {return xml_;}

	private:
		Xml xml_;
	};
}

#endif // pcw_XmlParserPage_hpp__


