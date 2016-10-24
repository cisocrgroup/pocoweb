#ifndef pcw_HocrPageParser_hpp__
#define pcw_HocrPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "PageParser.hpp"

namespace pugi {
	class xml_document;
	class xml_node;
}

namespace pcw {
	using Path = boost::filesystem::path;
	using XmlDocument = pugi::xml_document;
	using XmlDocumentPtr = std::unique_ptr<XmlDocument>;
	using XmlNode = pugi::xml_node;
	class Box;

	class HocrPageParser: virtual PageParser {
	public:
		static bool is_alto_document(const XmlDocument& xml);
	
		HocrPageParser(XmlDocumentPtr xml, Path path);
		virtual ~HocrPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr next() override;

	private:
		PagePtr parse() const;

		static PagePtr parse(const XmlNode& pagenode);
		static void add_line(Page& page, const XmlNode& linenode);
		static Box get_box(const XmlNode& node);

		const Path path_;
		const XmlDocumentPtr xml_;
		bool done_;
	};
}

#endif // pcw_HocrPageParser_hpp__
