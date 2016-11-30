#ifndef pcw_Profile_hpp__
#define pcw_Profile_hpp__

#include <boost/filesystem/path.hpp>
#include <regex>

namespace pcw {
	class Book;
	using ConstBookSptr = std::shared_ptr<const Book>;
	using Path = boost::filesystem::path;

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

	class Profile {

	private:
		friend class ProfileBuilder;
	};

	class ProfileBuilder {
	public:
		ProfileBuilder(ConstBookSptr book);
		void add_candidates_from_file(const Path& path);
		void add_candidate_from_string(const std::string& str);
		Profile build() const;

	private:
		ConstBookSptr book_;
	};
}

#endif // pcw_Profile_hpp__
