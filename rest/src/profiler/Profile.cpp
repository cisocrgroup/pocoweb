#include "Profile.hpp"
#include <utf8.h>
#include <iostream>
#include "core/Book.hpp"
#include "core/Page.hpp"
#include "pugixml.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
PatternExpr::PatternExpr(std::ssub_match mm) : left(), right(), pos() {
	static const std::regex patternre{R"((.*):(.*),(\d+))"};
	std::smatch m;
	if (not std::regex_match(mm.first, mm.second, m, patternre))
		THROW(ParseError, "Invalid pattern expression: ", mm);
	left = m[1];
	right = m[2];
	pos = std::stoul(m[3]);
}

////////////////////////////////////////////////////////////////////////////////
PatternsExpr::PatternsExpr(std::ssub_match mm) : patterns() {
	static const std::regex patternsre{R"(\((.*?,\d+)\)(.*))"};
	std::smatch m;
	while (std::regex_search(mm.first, mm.second, m, patternsre)) {
		patterns.emplace_back(m[1]);
		mm = m[2];
	}
}

////////////////////////////////////////////////////////////////////////////////
// ἕνος:{ἕνος+[]}+ocr[(ἕ:με,0)],voteWeight=0.0285641,levDistance=2
Explanation::Explanation(const std::string& expr) : hist(), histp(), ocrp() {
	static const std::regex explre{
	    R"((.*:)?\{(.+)\+\[(.*)\]\}\+ocr\[(.*)\].*)"};
	std::smatch m;
	if (not std::regex_match(expr, m, explre))
		THROW(ParseError, "Invalid explanation expression: ", expr);
	hist = m[2];
	histp = m[3];  // PatternsExpr(m[3]);
	ocrp = m[4];   // PatternsExpr(m[4]);
}

////////////////////////////////////////////////////////////////////////////////
// ἕνος:{ἕνος+[]}+ocr[(ἕ:με,0)],voteWeight=0.0285641,levDistance=2
Candidate::Candidate(const std::string& expr) : cor_(), expl_(), w_(), lev_() {
	static const std::regex candre{
	    R"((.*):(\{.*\+\[.*\]\}\+ocr\[.*\]),)"
	    R"(voteWeight=([-0-9.,eE]+),)"
	    R"(levDistance=(\d+))"};
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
    : cor_(std::move(cor)), expl_(std::move(expl)), w_(w), lev_(lev) {}

////////////////////////////////////////////////////////////////////////////////
std::wstring Candidate::wcor() const {
	std::wstring res;
	res.reserve(cor_.size());
	utf8::utf8to32(begin(cor_), end(cor_), std::back_inserter(res));
	return res;
}

////////////////////////////////////////////////////////////////////////////////
Profile::Patterns Profile::calc_ocr_patterns() const {
	Patterns ps;
	for (const auto& s : suggestions_) {
		for (const auto& cand : s.second) {
			const auto expl = cand.explanation();
			for (const auto& ocrp : expl.ocrp.patterns) {
				ps[ocrp].emplace(cand, s.first.cor());
			}
		}
	}
	return ps;
}

////////////////////////////////////////////////////////////////////////////////
Profile::Patterns Profile::calc_hist_patterns() const {
	Patterns ps;
	for (const auto& s : suggestions_) {
		for (const auto& cand : s.second) {
			const auto expl = cand.explanation();
			for (const auto& histp : expl.histp.patterns) {
				ps[histp].emplace(cand, s.first.cor());
			}
		}
	}
	return ps;
}

////////////////////////////////////////////////////////////////////////////////
ProfileBuilder::ProfileBuilder(ConstBookSptr book)
    : tokens_(), suggestions_(), book_(std::move(book)) {
	init();
}

////////////////////////////////////////////////////////////////////////////////
void ProfileBuilder::init() {
	for (const auto& page : *book_) {
		assert(page);
		for (const auto& line : *page) {
			assert(line);
			line->each_token([this](const auto& token) {
				auto id = token.unique_id();
				// std::cerr << "ADDING TOKEN:    " <<
				// token.ocr() << "\n";
				// std::cerr << "ADDING TOKEN ID: " <<
				// id <<
				// "\n";
				tokens_[id] = token;
			});
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void ProfileBuilder::clear() {
	suggestions_.clear();
	adaptive_tokens_.clear();
	// book remains untouched
	// tokens remains untouched
}

////////////////////////////////////////////////////////////////////////////////
Profile ProfileBuilder::build() const {
	Profile profile{book_};
	profile.suggestions_ = this->suggestions_;
	profile.adaptive_tokens_ = this->adaptive_tokens_;
	return profile;
}

////////////////////////////////////////////////////////////////////////////////
void ProfileBuilder::add_candidates_from_file(const Path& path) {
	pugi::xml_document doc;
	auto ok = doc.load_file(path.string().data());
	if (not ok)
		THROW(ParseError, "Could not parse file ", path, ": ",
		      ok.description());
	auto ts = doc.document_element().select_nodes(".//token");
	bool newtok = false;
	for (const auto& t : ts) {
		auto id = std::stoll(t.node().child("ext_id").child_value());
		auto token = tokens_[id];
		newtok = true;
		// std::cerr << "TOKEN ID:   " << id << "\n";
		// std::cerr << "TOKEN LINE: " << token.line.get() <<
		// "\n";
		// std::cerr << "TOKEN ID:   " << token.unique_id() <<
		// "\n";
		if (not token.line or not token.unique_id())
			THROW(ParseError, "Invalid token id: ", id);
		auto cs = t.node().select_nodes(".//cand");
		for (const auto& c : cs) {
			add_candidate_string(token, c.node().child_value(),
					     newtok);
			newtok = false;
		}
	}
	// adaptive tokens
	auto ats = doc.document_element().select_nodes(".//adaptiveToken");
	for (const auto& t : ats) {
		adaptive_tokens_.insert(t.node().child_value());
	}
}

////////////////////////////////////////////////////////////////////////////////
void ProfileBuilder::add_candidate_string(const Token& token,
					  const std::string& str, bool newtok) {
	if (not token.line or not token.unique_id())
		THROW(ParseError, "Invalid token for expression: ", str);
	add_candidate(token, Candidate(str), newtok);
}

////////////////////////////////////////////////////////////////////////////////
void ProfileBuilder::add_candidate(const Token& token, const Candidate& cand,
				   bool newtok) {
	if (not token.line or not token.unique_id())
		THROW(ParseError, "Encountered empty token for candidate: ",
		      cand.cor());
	// skip adaptive artifacts of the profiler:
	// <wOCR>Bomine</wOCR>
	// <wOCR_lc>bomine</wOCR_lc>
	// <wCorr>Homine</wCorr>
	// <cand>Homine:{homine+[]}+ocr[(h:b,0)],voteWeight=1,levDistance=1</cand>
	if (cand.lev() > 0 and cand.cor() != token.cor()) {
		suggestions_[token].insert(cand);
	}
}
