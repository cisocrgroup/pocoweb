#ifndef pcw_OcropusLlocsPageParser_hpp__
#define pcw_OcropusLlocsPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <memory>
#include "PageParser.hpp"

namespace pcw {
	class ParserPage;
	using ParserPagePtr = std::shared_ptr<ParserPage>;
	class Line;
	using LinePtr = std::shared_ptr<Line>;
	using Path = boost::filesystem::path;

	class OcropusLlocsPageParser: public PageParser {
	public:
		OcropusLlocsPageParser(Path path);
		virtual ~OcropusLlocsPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual ParserPagePtr parse() override;

	private:
		using PathPair = std::pair<Path, Path>;
		ParserPagePtr parse_page() const;

		static boost::optional<PathPair> get_path_pair(const Path& file);

		const Path dir_;
		const int id_;
		bool done_;
	};
}

#endif // pcw_OcropusLlocsPageParser_hpp__
