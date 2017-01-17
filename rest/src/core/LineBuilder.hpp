#ifndef pcw_LineBuilder_hpp__
#define pcw_LineBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
	class Box;
	class Line;
	using LineSptr = std::shared_ptr<Line>;
	class Page;
	using PageSptr = std::shared_ptr<Page>;
	class Box;
	using Path = boost::filesystem::path;

	class LineBuilder {
	public:
		LineBuilder();

		LineBuilder& reset();
		LineBuilder& append(const std::string& str, int l, int r, double c);
		LineBuilder& append(const std::wstring& str, int l, int r, double c);
		LineBuilder& append(const char* str, int l, int r, double );
		LineBuilder& append(const wchar_t* str, int l, int r, double c);
		LineBuilder& append(const char* str, size_t n, int l, int r, double c);
		LineBuilder& append(const wchar_t* str, size_t n, int l, int r, double c);
		LineBuilder& append(wchar_t c, int r, double conf);
		LineBuilder& append(wchar_t o, wchar_t c, int r, double conf);
		LineBuilder& set_image(Path img);
		LineBuilder& set_box(Box box);
		LineBuilder& set_page(PageSptr page);
		LineSptr build() const;

	private:
		std::vector<std::tuple<wchar_t, wchar_t, int, double>> contents_;
		LineSptr line_;
		PageSptr page_;
	};
}

#endif //pcw_LineBuilder_hpp__
