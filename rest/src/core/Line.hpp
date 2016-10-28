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
		using String = std::wstring;
		using Cuts = std::vector<int>;
		using Confidences = std::vector<double>;
	
		Line(int i, Box box = {});
	
		bool empty() const noexcept {return string_.empty();}
		size_t size() const noexcept {return string_.size();}
		const String& wstring() const noexcept {return string_;}
		std::string string() const;
		const Cuts& cuts() const noexcept {return cuts_;}
		const Confidences& confidences() const noexcept {return confs_;}
		double calculate_average_confidence() const noexcept;
		PagePtr page() const noexcept {return page_.lock();}
		bool has_img_path() const noexcept {return not img.empty();}

		void append(const std::string& str, int l, int r, double c);
		void append(const std::wstring& str, int l, int r, double c);
		void append(const char* str, int l, int r, double c);
		void append(const wchar_t* str, int l, int r, double c);
		void append(const char* str, size_t n, int l, int r, double c);
		void append(const wchar_t* str, size_t n, int l, int r, double c);
		void append(wchar_t c, int r, double conf);

		Box box;
		int id;
		Path img, ocr;

	private:
		String string_;
		Cuts cuts_;
		Confidences confs_;
		std::weak_ptr<Page> page_;
		friend class Page;
	};
}

#endif // pcw_Line_hpp__
