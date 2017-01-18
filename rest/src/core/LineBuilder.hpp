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
		const LineBuilder& append(const std::string& str, int r, double c) const;
		const LineBuilder& append(const std::wstring& str, int r, double c) const;
		const LineBuilder& append(wchar_t c, int r, double conf) const;
		const LineBuilder& append(wchar_t o, wchar_t c, int r, double conf) const;
		const LineBuilder& set_image_path(Path img) const;
		const LineBuilder& set_box(Box box) const;
		LineSptr build() const;

	private:
		LineSptr line_;
	};
}

#endif //pcw_LineBuilder_hpp__
