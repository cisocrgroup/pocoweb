#ifndef pcw_XmlPageParser_hpp__
#define pcw_XmlPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pugi {
	class xml_node;
	class xml_document;
}

namespace pcw {
	using Path = boost::filesystem::path;
	class XmlPageParser {
	public:
		using Doc = pugi::xml_document;
		using Node = pugi::xml_node;
		using DocPtr = std::shared_ptr<pugi::xml_document>;

		XmlPageParser(DocPtr doc = nullptr);
		XmlPageParser(const Path& path);
		virtual ~XmlPageParser() noexcept = default;
		Doc& doc() noexcept {return *doc_;}
		const Doc& doc() const noexcept {return *doc_;}
		DocPtr& doc_ptr() noexcept {return doc_;}
		const DocPtr& doc_ptr() const noexcept {return doc_;}

	protected:
		void read(const Path& path);
		DocPtr doc_;
	};
}

#endif // pcw_XmlPageParser_hpp__
