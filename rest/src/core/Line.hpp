#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"

namespace pcw {
	struct EditOp;
	using EditOps = std::vector<EditOp>;
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Line {
	public:
		using Path = boost::filesystem::path;
		using String = std::wstring;
		using Cuts = std::vector<int>;
		using Confidences = std::vector<double>;
		using Corrections = std::vector<bool>;
		using EditOps = std::vector<EditOp>;
		struct Word {
			Box box;
			std::string word;
			double confidence;
			bool corrected;
		};
	
		Line(int i, Box box = {});
	
		bool empty() const noexcept {return string_.empty();}
		size_t size() const noexcept {return string_.size();}
		const String& wstring() const noexcept {return string_;}
		std::string string() const;
		const Cuts& cuts() const noexcept {return cuts_;}
		const Confidences& confidences() const noexcept {return confs_;}
		const Corrections& corrections() const noexcept {return corrs_;}
		double calculate_average_confidence() const noexcept;
		PagePtr page() const noexcept {return page_.lock();}
		bool has_img_path() const noexcept {return not img.empty();}
		void correct(const EditOps& edits, size_t offset);
		void each_word(std::function<void(const Word&)> f) const;

		void append(const std::string& str, int l, int r, double c, bool corr = false);
		void append(const std::wstring& str, int l, int r, double c, bool corr = false);
		void append(const char* str, int l, int r, double c, bool corr = false);
		void append(const wchar_t* str, int l, int r, double c, bool corr = false);
		void append(const char* str, size_t n, int l, int r, double c, bool corr = false);
		void append(const wchar_t* str, size_t n, int l, int r, double c, bool corr = false);
		void append(wchar_t c, int r, double conf, bool corr = false);

		Box box;
		int id;
		Path img, ocr;

	private:
		void delete_at(size_t i);
		void insert_at(size_t i, wchar_t c);

		String string_;
		Cuts cuts_;
		Confidences confs_;
		Corrections corrs_;
		std::weak_ptr<Page> page_;
		friend class Page;
	};
}

#endif // pcw_Line_hpp__
