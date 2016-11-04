#ifndef pcw_ParserChar_hpp__
#define pcw_ParserChar_hpp__

#include <memory>
#include "core/Box.hpp"

namespace pcw {
	class ParserWord;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;
	using ParserWordPtr = std::shared_ptr<ParserWord>;

	class ParserChar: public std::enable_shared_from_this<ParserChar> {
	public:
		ParserChar(Box box = {}, wchar_t c = 0, double conf = 0)
			: box_(box) 
			, char_(c)
			, conf_(conf)
		{}
		virtual ~ParserChar() noexcept = default;
		virtual ParserCharPtr set(wchar_t c) = 0;
		virtual void remove() = 0;
		virtual ParserCharPtr insert(wchar_t c) = 0;

		wchar_t get() const noexcept {return char_;}
		const Box& box() const noexcept {return box_;}
		double conf() const noexcept {return conf_;}

	protected:
		Box box_;
		wchar_t char_;
		double conf_;
	};

	class ParserWordChar: public ParserChar {
	public:
		ParserWordChar(
			Box box = {}, 
			wchar_t c = 0, 
			double conf = 0, 
			ParserWordPtr word = nullptr
		)
			: ParserChar(box, c, conf)
			, word_(word)
		{}
		virtual ~ParserWordChar() noexcept override = default;
		virtual ParserCharPtr set(wchar_t c) override;
		virtual void remove() override;
		virtual ParserCharPtr insert(wchar_t c) override;
		const ParserWordPtr& word() const noexcept {return word_;}
		void set_word(ParserWordPtr word) noexcept {word_ = std::move(word);}

	private:
		ParserWordPtr word_;
	};
}


#endif // pcw_ParserChar_hpp__
