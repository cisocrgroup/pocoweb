#ifndef pcw_LlocsPageParser_hpp__
#define pcw_LlocsPageParser_hpp__

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <memory>
#include "PageParser.hpp"

namespace pcw {
	class Line;
	using Path = boost::filesystem::path;

	class LlocsPageParser: virtual PageParser {
	public:
		LlocsPageParser(Path path);
		virtual ~LlocsPageParser() noexcept override = default;
		virtual bool has_next() const noexcept override {return not done_;}
		virtual PagePtr next() override;

	private:
		using PathPair = std::pair<Path, Path>;
		PagePtr parse() const;

		static boost::optional<PathPair> get_path_pair(const Path& file);
		static Line parse_line(int i, const PathPair& pair);

		const Path dir_;
		const int id_;
		bool done_;
	};
}

#endif // pcw_LlocsPageParser_hpp__
