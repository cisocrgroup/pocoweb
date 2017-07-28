#ifndef pcw_Profile_hpp__
#define pcw_Profile_hpp__

#include <boost/filesystem/path.hpp>
#include <map>
#include <regex>
#include <set>
#include <unordered_map>
#include "core/Book.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/Project.hpp"

namespace pcw {
using ConstBookSptr = std::shared_ptr<const Book>;
using Path = boost::filesystem::path;
struct Char;

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
	Candidate() : Candidate("", 0, 0, "") {}
	Candidate(const std::string& expr);
	Candidate(std::string cor, double w, int lev, std::string expl_);

	const std::string& cor() const noexcept { return cor_; }
	std::wstring wcor() const;
	double weight() const noexcept { return w_; }
	int lev() const noexcept { return lev_; }
	Explanation explanation() const { return expl_; }
	const std::string& explanation_string() const noexcept { return expl_; }

       private:
	std::string cor_;
	std::string expl_;
	double w_;
	int lev_;
	friend bool operator==(const Candidate&, const Candidate&) noexcept;
	friend bool operator<(const Candidate&, const Candidate&) noexcept;
};

struct Suggestion {
	Suggestion(const Candidate& c, const std::string& t)
	    : cand(c), ocr(t) {}
	Candidate cand;
	std::string ocr;
};

struct Pattern {
	Pattern(const PatternExpr& e) : ocr(e.ocr), cor(e.cor) {}
	Pattern(std::string c, std::string o)
	    : ocr(std::move(o)), cor(std::move(c)) {}
	std::string ocr, cor;
};

static inline bool operator==(const Pattern& lhs, const Pattern& rhs) noexcept {
	return std::tie(lhs.ocr, lhs.cor) == std::tie(rhs.ocr, rhs.cor);
}

static inline bool operator<(const Pattern& lhs, const Pattern& rhs) noexcept {
	return std::tie(lhs.ocr, lhs.cor) < std::tie(rhs.ocr, rhs.cor);
}

inline bool operator==(const Candidate& a, const Candidate& b) noexcept {
	return std::tie(a.w_, a.lev_, a.cor_, a.expl_) ==
	       std::tie(b.w_, b.lev_, b.cor_, b.expl_);
}

inline bool operator<(const Candidate& a, const Candidate& b) noexcept {
	return std::tie(a.w_, a.lev_, a.cor_, a.expl_) <
	       std::tie(b.w_, b.lev_, b.cor_, b.expl_);
}

static inline bool operator==(const Suggestion& a,
			      const Suggestion& b) noexcept {
	return std::tie(a.cand, a.ocr) == std::tie(b.cand, b.ocr);
}

static inline bool operator<(const Suggestion& a,
			     const Suggestion& b) noexcept {
	return std::tie(a.cand, a.ocr) < std::tie(b.cand, b.ocr);
}

class Profile {
       public:
	using Suggestions = std::map<std::string, std::set<Candidate>>;
	using Patterns = std::map<Pattern, std::set<Suggestion>>;

	Profile(ConstBookSptr book) : suggestions_(), book_(std::move(book)) {}

	const Book& book() const noexcept { return *book_; }
	const Suggestions& suggestions() const noexcept { return suggestions_; }
	size_t count(const std::string& str) const noexcept;
	Patterns calc_ocr_patterns() const;
	Patterns calc_hist_patterns() const;

       private:
	std::map<std::string, size_t> counts_;
	Suggestions suggestions_;
	ConstBookSptr book_;

	friend class ProfileBuilder;
};

class ProfileBuilder {
       public:
	ProfileBuilder(ConstBookSptr book);
	void clear();
	Profile build() const;
	void add_candidates_from_file(const Path& path);
	void add_candidate_string(const Token& token, const std::string& str,
				  bool newttok = true);
	void add_candidate(const Token& token, const Candidate& cand,
			   bool newttok = true);

       private:
	void init();

	std::unordered_map<int64_t, Token> tokens_;
	std::map<std::string, size_t> counts_;
	Profile::Suggestions suggestions_;
	ConstBookSptr book_;
};
}

#endif  // pcw_Profile_hpp__
