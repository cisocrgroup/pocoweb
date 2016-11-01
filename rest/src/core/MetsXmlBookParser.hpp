#ifndef pcw_MetsXmlBookParser_hpp__
#define pcw_MetsXmlBookParser_hpp__

#include "BookParser.hpp"
#include "XmlFile.hpp"

namespace pcw {
	class MetsXmlBookParser: public BookParser, 
				 public XmlFile,
				 public pugi::xml_tree_walker {
	public:
		MetsXmlBookParser(const Path& path);
		virtual ~MetsXmlBookParser() noexcept override = default;
		virtual BookPtr parse() override;
		virtual bool for_each(XmlNode& node) override;

	private:
		BookPtr book_;
	};
}

#endif // pcw_MetsXmlBookParser_hpp__
