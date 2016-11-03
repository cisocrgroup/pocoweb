#ifndef pcw_ParserWord_hpp__
#define pcw_ParserWord_hpp__

#include <memory>
#include <vector>

namespace pcw {
	class ParserWordChar;
	class ParserWord;
	using ParserWordPtr = std::shared_ptr<ParserWord>;
	using ParserWordCharPtr = std::shared_ptr<ParserWordChar>;

	class ParserWord: public std::enable_shared_from_this<ParserWord> {
	public:
		virtual ~ParserWord() noexcept = default;
		virtual void update() = 0;
		virtual void remove() = 0;
		void remove(ParserWordChar* c);
		void insert(ParserWordChar* new_char, ParserWordChar* at);
		void merge(ParserWordChar* at, ParserWord& word);

	protected:
		std::vector<ParserWordChar*> chars_;
	};
}

#endif // pcw_ParserWord_hpp__
