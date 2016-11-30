#include <iostream>
#include <utf8.h>
#include "Error.hpp"
#include "Profile.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
PatternExpr::PatternExpr(std::ssub_match mm)
	: ocr()
	, cor()
	, pos()
{
	static const std::regex patternre{R"((.*):(.*),(\d+))"};
	std::smatch m;
	if (not std::regex_match(mm.first, mm.second, m, patternre))
		THROW(ParseError, "Invalid pattern expression: ", mm);
	ocr = m[2];
	cor = m[1];
	pos = std::stoul(m[3]);
}

////////////////////////////////////////////////////////////////////////////////
PatternsExpr::PatternsExpr(std::ssub_match mm)
	: patterns()
{
	static const std::regex patternsre{R"(\((.*?,\d+)\)(.*))"};
	std::smatch m;
	while (std::regex_search(mm.first, mm.second, m, patternsre)) {
		patterns.emplace_back(m[1]);
		mm = m[2];
	}
}

////////////////////////////////////////////////////////////////////////////////
// ἕνος:{ἕνος+[]}+ocr[(ἕ:με,0)],voteWeight=0.0285641,levDistance=2
Explanation::Explanation(const std::string& expr)
	: hist()
	, histp()
	, ocrp()
{
	static const std::regex explre{
		R"((.*:)?\{(.+)\+\[(.*)\]\}\+ocr\[(.*)\].*)"
	};
	std::smatch m;
	if (not std::regex_match(expr, m, explre))
		THROW(ParseError, "Invalid explanation expression: ", expr);
	hist = m[2];
	histp = m[3]; // PatternsExpr(m[3]);
	ocrp = m[4]; // PatternsExpr(m[4]);
}

////////////////////////////////////////////////////////////////////////////////
// ἕνος:{ἕνος+[]}+ocr[(ἕ:με,0)],voteWeight=0.0285641,levDistance=2
Candidate::Candidate(const std::string& expr)
	: cor_()
	, expl_()
	, w_()
	, lev_()
{
	static const std::regex candre{
		R"((.*):(\{.*\+\[.*\]\}\+ocr\[.*\]),)"
		R"(voteWeight=(\d*[.,]\d+),)"
		R"(levDistance=(\d+))"
	};
	std::smatch m;
	if (not std::regex_match(expr, m, candre))
		THROW(ParseError, "Invalid candidate expression: ", expr);
	cor_ = m[1];
	expl_ = m[2];
	w_ = std::stod(m[3]);
	lev_ = std::stoi(m[4]);
}

////////////////////////////////////////////////////////////////////////////////
Candidate::Candidate(std::string cor, double w, int lev, std::string expl)
	: cor_(std::move(cor))
	, expl_(std::move(expl))
	, w_(w)
	, lev_(lev)
{
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
Candidate::wcor() const
{
	std::wstring res;
	res.reserve(cor_.size());
	utf8::utf8to32(begin(cor_), end(cor_), std::back_inserter(res));
	return res;
}

////////////////////////////////////////////////////////////////////////////////
ProfileBuilder::ProfileBuilder(ConstBookSptr book)
	: book_(std::move(book))
{
}

////////////////////////////////////////////////////////////////////////////////
Profile
ProfileBuilder::build() const
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
void
ProfileBuilder::add_candidates_from_file(const Path& path)
{

}

////////////////////////////////////////////////////////////////////////////////
void
ProfileBuilder::add_candidate_from_string(const std::string& str)
{
}

