#ifndef pcw_LineBuilder_hpp__
#define pcw_LineBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
	class Box;
	class Line;
	using LineSptr = std::shared_ptr<Line>;
	class Box;
	using Path = boost::filesystem::path;

	class LineBuilder {
	public:
		LineBuilder(): line_() {reset();}

		LineBuilder& reset();
		LineBuilder& append(const std::string& str, int r, double c);
		LineBuilder& append(const std::wstring& str, int r, double c);
		LineBuilder& append(wchar_t c, int r, double conf);
		LineBuilder& append(wchar_t o, wchar_t c, int r, double conf);
		LineBuilder& set_image_path(Path img);
		LineBuilder& set_box(Box box);
		LineSptr build() const;

	private:
		LineSptr line_;
	};
}

#endif //pcw_LineBuilder_hpp__
