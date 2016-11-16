#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "Box.hpp"

namespace pcw {
	class Page;
	using PagePtr = std::shared_ptr<Page>;
	class WagnerFischer;

	class Line {
	public:
		struct Char;
		using Chars = std::vector<Char>;
		using CharIterator = Chars::const_iterator;
		using Path = boost::filesystem::path;
		using String = std::wstring;
		using Cuts = std::vector<int>;
		using Confidences = std::vector<double>;
		using Corrections = std::vector<bool>;

		struct Token {
			Token() = default;
			Token(CharIterator b, CharIterator e, Box bbox = {})
				: range(b, e)
				, box(bbox)
			{}

			double average_conf() const;
			std::wstring wcor() const;
			std::wstring wocr() const;
			std::string cor() const;
			std::string ocr() const;
			bool is_corrected() const;
			bool is_normal() const;

			std::pair<CharIterator, CharIterator> range;
			Box box;
		};

		struct Char {
			static const wchar_t DELETION = 0xffffffff;
			Char() = default;
			Char(wchar_t o, wchar_t c, int ccut, double cconf)
				: ocr(o)
				, cor(c)
				, conf(cconf)
				, cut(ccut)
			{}
			bool is_deletion() const noexcept {return cor == DELETION;} 
			bool is_insertion() const noexcept {return cor and not ocr;}
			bool is_substitution() const noexcept {return cor and ocr and cor != ocr;}
			bool is_corrected() const noexcept {return cor;}
			bool is_word() const noexcept;
			bool is_sep() const noexcept;
			wchar_t get_cor() const noexcept {
				return is_deletion() ? 0 : is_corrected() ? cor : ocr;
			}

			wchar_t ocr, cor;
			double conf;
			int cut;
		};

		Line(int i, Box box = {});

		bool empty() const noexcept {return chars_.empty();}
		size_t size() const noexcept {return chars_.size();}
		std::wstring wocr() const;
		std::string ocr() const;
		std::wstring wcor() const;
		std::string cor() const;
		Cuts cuts() const;
		Confidences confidences() const;
		const Chars& chars() const noexcept {return chars_;}
		const Char& operator[](size_t i) const noexcept {return chars_[i];}
		double average_conf() const noexcept;

		PagePtr page() const noexcept {return page_.lock();}
		bool has_img_path() const noexcept {return not img.empty();}
		void each_token(std::function<void(const Token&)> f) const;
		std::vector<Token> tokens() const;
		std::vector<Token> words() const;
		int id() const noexcept {return id_;}

		void append(const std::string& str, int l, int r, double c);
		void append(const std::wstring& str, int l, int r, double c);
		void append(const char* str, int l, int r, double );
		void append(const wchar_t* str, int l, int r, double c);
		void append(const char* str, size_t n, int l, int r, double c);
		void append(const wchar_t* str, size_t n, int l, int r, double c);
		void append(wchar_t c, int r, double conf);
		void append(wchar_t o, wchar_t c, int r, double conf);

		// interface for WagnerFischer correction
		void begin_wagner_fischer() noexcept;
		void insert(size_t i, wchar_t c);
		void erase(size_t i);
		void set(size_t i, wchar_t c);
		void noop(size_t i);
		void end_wagner_fischer() const noexcept {}

		Box box;
		Path img;

	private:
		void divide_cuts(Chars::iterator f, Chars::iterator l);

		template<class It, class F>
		static void cor(It b, It e, F f) {
			while (b != e) {
				if (b->get_cor())
					f(*b);
				++b;
			}
		}
		template<class It, class F>
		static void ocr(It b, It e, F f) {
			while (b != e) {
				if (b->ocr)
					f(*b);
				++b;
			}
		}

		Chars chars_;
		std::weak_ptr<Page> page_;
		int id_, ofs_;
		friend class Page;
	};
}

#endif // pcw_Line_hpp__
