#ifndef pcw_Profile_hpp__
#define pcw_Profile_hpp__

#include <boost/filesystem/path.hpp>
#include <regex>
#include <unordered_map>
#include "Line.hpp"

namespace pcw {
	class Book;
	using ConstBookSptr = std::shared_ptr<const Book>;
	using Path = boost::filesystem::path;
	class Char;

	struct PatternExpr {
		PatternExpr() = default;
		PatternExpr(std::ssub_match mm);
		bool empty() const noexcept {
			return ocr.length() == 0 and cor.length() == 0;
		}
		std::ssub_match ocr, cor;
		size_t pos;
	};

	struct PatternsExpr {
		PatternsExpr() = default;
		PatternsExpr(std::ssub_match mm);
		std::vector<PatternExpr> patterns;
	};

	struct Explanation {
		Explanation(const std::string& expr);
		std::ssub_match hist;
		PatternsExpr histp, ocrp;
	};

	class Candidate {
	public:
		Candidate(const std::string& expr);
		Candidate(std::string cor, double w, int lev, std::string expl_);

		const std::string& cor() const noexcept {return cor_;}
		std::wstring wcor() const;
		double weight() const noexcept {return w_;}
		int lev() const noexcept {return lev_;}
		Explanation explanation() const {return expl_;}
		const std::string& explanation_string() const noexcept {return expl_;}

	private:
		std::string cor_;
		std::string expl_;
		double w_;
		int lev_;
	};

	struct Suggestion {
		Suggestion(const Candidate& c, const Token& t)
			: cand(c)
			, token(t)
		{}
		Candidate cand;
		Token token;
	};

	struct Pattern {
		Pattern(const PatternExpr& e)
			: ocr(e.ocr)
			, cor(e.cor)
		{}
		Pattern(std::string o, std::string c)
			: ocr(std::move(o))
			, cor(std::move(c))
		{}
		std::string ocr, cor;
	};
	static inline bool operator==(const Pattern& lhs,
			const Pattern& rhs) noexcept {
		return std::tie(lhs.ocr, lhs.cor) == std::tie(rhs.ocr, rhs.cor);
	}
	static inline bool operator<(const Pattern& lhs,
			const Pattern& rhs) noexcept {
		return std::tie(lhs.ocr, lhs.cor) < std::tie(rhs.ocr, rhs.cor);
	}

	class Profile {
	public:
		using Suggestions = std::vector<Suggestion>;
		using Patterns = std::map<Pattern, Suggestion>;

		Profile(ConstBookSptr book)
			: suggestions_()
			, book_(std::move(book))
		{}

		const Suggestions& suggestions() const noexcept {
			return suggestions_;
		}
		Patterns calc_ocr_patterns() const;
		Patterns calc_hist_patterns() const;

	private:
		Suggestions suggestions_;
		ConstBookSptr book_;

		friend class ProfileBuilder;
	};

	class ProfileBuilder {
	public:
		ProfileBuilder(ConstBookSptr book);
		Profile build() const;
		void add_candidates_from_file(const Path& path);
		void add_candidate_string(const Token& token, const std::string& str);
		void add_candidate(const Token& token, const Candidate& cand);

	private:
		void init();

		std::unordered_map<int64_t, Token> tokens_;
		Profile::Suggestions suggestions_;
		ConstBookSptr book_;
	};
}

#endif // pcw_Profile_hpp__
