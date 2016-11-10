#ifndef pcw_XmlParserPage_hpp__
#define pcw_XmlParserPage_hpp__

#include <memory>
#include "ParserPage.hpp"
#include "Xml.hpp"

namespace pcw {
	class XmlParserPage: public ParserPage {
	public:
		using Node = pugi::xml_node;
		using Doc = pugi::xml_document;
		using DocPtr = std::shared_ptr<Doc>;

		XmlParserPage(Xml xml = {}): lines_(), xml_(xml) {}
		XmlParserPage(const Path& path): lines_(), xml_(path) {}
		virtual ~XmlParserPage() noexcept override = default;
		virtual void write(const Path& path) const override;
		virtual size_t size() const noexcept override {
			return lines_.size();
		}
		virtual ParserLine& get(size_t i) const override {
			return *lines_[i];
		}
		virtual ParserLine& get(size_t i) override {
			return *lines_[i];
		}
		Xml& xml() noexcept {return xml_;}
		const Xml& xml() const noexcept {return xml_;}
		const std::vector<ParserLinePtr>& lines() const noexcept {
			return lines_;
		}
		std::vector<ParserLinePtr>& lines() noexcept {
			return lines_;
		}

	private:
		std::vector<ParserLinePtr> lines_;
		Xml xml_;
	};
}

#endif // pcw_XmlParserPage_hpp__


