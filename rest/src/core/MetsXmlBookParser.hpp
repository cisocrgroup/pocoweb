#ifndef pcw_MetsXmlBookParser_hpp__
#define pcw_MetsXmlBookParser_hpp__

#include <map>
#include <vector>
#include "util.hpp"
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
		virtual bool begin(XmlNode& node) override;
		virtual bool for_each(XmlNode& node) override;

	private:
		void make_book();

		static bool equal(const char* uri, const char* str) noexcept;
		static bool equal_ns(const char* uri, const char* str) noexcept;
		static bool is_flocat(const XmlNode& node) noexcept;
		static bool is_area(const XmlNode& node) noexcept;
		static const char* attr_value_ns(const XmlNode& node, const char* name) noexcept;
		
		struct FileGroup {
			FileGroup() = default;
			FileGroup(const char *mimetype, Path path);
			Path href;
			FileType type;
		};
		std::map<std::string, FileGroup> ids_;
		std::map<int, std::vector<std::string>> map_;
		BookPtr book_;
		const Path base_;
	};
}

#endif // pcw_MetsXmlBookParser_hpp__
