#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"

namespace pcw {
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Line {
	public:
		using Path = boost::filesystem::path;
		using String = std::string;
		using Cuts = std::vector<int>;
		using Confidences = std::vector<double>;
	
		Line(Page& page, int i, Box box);
	
		bool empty() const noexcept {return string_.empty();}
		size_t size() const noexcept {return string_.size();}
		const String& string() const noexcept {return string_;}
		const Cuts& cuts() const noexcept {return cuts_;}
		const Confidences& confidences() const noexcept {return confs_;}

		void append(const char* str, int l, int r, double c);
		void append(char c, int l, int r, double conf);
		void append(const wchar_t* str, int l, int r, double c);
		void append(wchar_t c, int l, int r, double conf);

		const std::weak_ptr<Page> page;
		const Box box;
		const int id;
		Path img, ocr;

	private:
		String string_;
		Cuts cuts_;
		Confidences confs_;
	};
}

#endif // pcw_Line_hpp__
