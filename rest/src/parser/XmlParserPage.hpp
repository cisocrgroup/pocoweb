#ifndef pcw_XmlParserPage_hpp__
#define pcw_XmlParserPage_hpp__

#include "ParserPage.hpp"

namespace pugi {
	class xml_document;
	class xml_node;
}

namespace pcw {
	class XmlParserPage: public ParserPage {
	public:
		XmlParserPage(const Path& path);
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
		pugi::xml_document& doc() noexcept {
			return *doc_;
		}
		const pugi::xml_document& doc() const noexcept {
			return *doc_;
		}
		const std::vector<ParserLinePtr>& lines() const noexcept {
			return lines_;
		}
		std::vector<ParserLinePtr>& lines() noexcept {
			return lines_;
		}
	
	protected:
		std::vector<ParserLinePtr> lines_;
		std::shared_ptr<pugi::xml_document> doc_;

	};
}

#endif // pcw_XmlParserPage_hpp__


