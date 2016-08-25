#ifndef pcw_Page_hpp__
#define pcw_Page_hpp__

#include <boost/filesystem/path.hpp>
#include "Container.hpp"
#include "Line.hpp"
#include "Box.hpp"

namespace pcw {
	using Path = boost::filesystem::path;

	class Page: public std::enable_shared_from_this<Page>,
		    public Container<Line> {
	public:
		Page() = default;
		Page(nlohmann::json& json): Page() {load(json);}
		virtual ~Page() noexcept override = default;
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;
		void parse();

		Box box;
		Path imagefile, ocrfile;
		int id;
	};
	using PagePtr = std::shared_ptr<Page>;
}

#endif // pcw_Page_hpp__

