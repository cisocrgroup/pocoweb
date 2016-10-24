#ifndef pcw_AbbyyXmlPageParser_hpp__
#define pcw_AbbyyXmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include "PageParser.hpp"
#include "XmlFile.hpp"

namespace pcw {
	class Box;

	class AbbyyXmlPageParser: public PageParser, 
				  public XmlFile {
	public:
		static bool is_alto_document(const XmlDocument& xml);
	
		AbbyyXmlPageParser(const Path& path);
		virtual ~AbbyyXmlPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override;
		virtual PagePtr next() override;

	private:
		XmlNode page_;
	};
}

#endif // pcw_AbbyyXmlPageParser_hpp__
