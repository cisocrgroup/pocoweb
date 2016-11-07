#ifndef pcw_ParserWord_hpp__
#define pcw_ParserWord_hpp__

#include <memory>
#include <vector>
#include "core/Box.hpp"

namespace pcw {
	class ParserWordChar;
	class ParserWord;
	using ParserWordPtr = std::shared_ptr<ParserWord>;
	using ParserWordCharPtr = std::shared_ptr<ParserWordChar>;

	class ParserWord: public std::enable_shared_from_this<ParserWord> {
	public:
		virtual ~ParserWord() noexcept = default;

		void update();
		void remove(ParserWordChar* c);
		void push_back(ParserWordChar* new_char);
		void push_front(ParserWordChar* new_char);
		void insert(ParserWordChar* new_char, ParserWordChar* at);
		void merge(ParserWordChar* at, ParserWord& word);
		void split(ParserWordChar* at);
		const Box& box() const noexcept {return box_;}
		double conf() const noexcept {return conf_;}

	protected:
		virtual ParserWordPtr create() = 0;
		virtual void update(const std::string& word) = 0;
		virtual void remove() = 0;

		Box box_;
		std::vector<ParserWordChar*> chars_;
		double conf_;
	};
}

#endif // pcw_ParserWord_hpp__
