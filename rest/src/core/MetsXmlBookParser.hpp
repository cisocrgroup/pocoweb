#ifndef pcw_MetsXmlBookParser_hpp__
#define pcw_MetsXmlBookParser_hpp__

#include "BookParser.hpp"
#include "XmlFile.hpp"

namespace pcw {
	class MetsXmlBookParser: public BookParser, 
			      public XmlFile {
	public:
		MetsXmlBookParser(const Path& path): XmlFile(path) {}
		virtual ~MetsXmlBookParser() noexcept override = default;
		virtual BookPtr parse() const override;
	};
}

#endif // pcw_MetsXmlBookParser_hpp__
