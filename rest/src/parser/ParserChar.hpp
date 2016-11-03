#ifndef pcw_ParserChar_hpp__
#define pcw_ParserChar_hpp__

#include <memory>

namespace pcw {
	class ParserWord;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;
	using ParserWordPtr = std::shared_ptr<ParserWord>;

	class ParserChar: public std::enable_shared_from_this<ParserChar> {
	public:
		virtual ~ParserChar() noexcept = default;
		virtual wchar_t get() const = 0;
		virtual void set(wchar_t) = 0;
		virtual void remove() = 0;
		virtual ParserCharPtr clone() const = 0;
	};

	class BasicParserChar: public ParserChar {
	public:
		BasicParserChar(wchar_t c = 0, double conf = 0, int cut = 0);
		virtual ~BasicParserChar() noexcept override = default;
		virtual wchar_t get() const override {return char_;}
		virtual void set(wchar_t c) override {char_ = c;}
		virtual void remove() override {}
		virtual ParserCharPtr clone() const override;

	protected:
		wchar_t char_;
		double conf_;
		int cut_;

	};
	
	class ParserWordChar: public BasicParserChar {
	public:
		virtual ~ParserWordChar() noexcept override = default;
		ParserWord& word() const {return *word_;}
		void set_word(ParserWordPtr word) {word_ = std::move(word);}

	private:
		ParserWordPtr word_;
	};
}


#endif // pcw_ParserChar_hpp__
