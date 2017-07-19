#include "WagnerFischer.hpp"
#include <crow/logging.h>
#include <utf8.h>
#include <algorithm>
#include <cassert>
#include <cwchar>
#include <iomanip>
#include <numeric>
#include "Line.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const std::string& gt) {
	set_gt(gt.data(), gt.size());
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const char* gt) { set_gt(gt, strlen(gt)); }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const char* gt, size_t n) {
	gt_.clear();
	gt_.reserve(std::max(n * 2, gt_.capacity()));
	utf8::utf8to32(gt, gt + n, std::back_inserter(gt_));
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const std::wstring& gt) { gt_ = gt; }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const wchar_t* gt) { gt_ = std::wstring(gt); }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const wchar_t* gt, size_t n) {
	gt_ = std::wstring(gt, n);
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const Line& line) { gt_ = line.wcor(); }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_gt(const std::wregex& pat, const std::wstring& repl) {
	if (gt_.empty()) {
		std::regex_replace(std::back_inserter(gt_), begin(ocr_),
				   end(ocr_), pat, repl.data());
	} else {
		std::wstring tmp = gt_;
		gt_.clear();
		std::regex_replace(std::back_inserter(gt_), begin(tmp),
				   end(tmp), pat, repl.data());
	}
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const std::string& ocr) {
	set_ocr(ocr.data(), ocr.size());
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const char* ocr) { set_ocr(ocr, strlen(ocr)); }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const char* ocr, size_t n) {
	ocr_.clear();
	ocr_.reserve(std::max(n * 2, ocr_.capacity()));
	utf8::utf8to32(ocr, ocr + n, std::back_inserter(ocr_));
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const std::wstring& ocr) { ocr_ = ocr; }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const wchar_t* ocr) { ocr_ = std::wstring(ocr); }

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const wchar_t* ocr, size_t n) {
	ocr_ = std::wstring(ocr, n);
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::set_ocr(const Line& line) {
	// use the corrected version as well
	// (if line is not corrected, wcor contains all ocr chars)
	ocr_ = line.wcor();
}

////////////////////////////////////////////////////////////////////////////////
size_t WagnerFischer::operator()() {
	const auto gtn = gt_.size();
	const auto ocrn = ocr_.size();
	l_.clear();
	l_.emplace_back(gtn + 1, 0);
	std::iota(l_[0].begin(), l_[0].end(), 0);
	for (auto i = 1U; i < ocrn + 1; ++i) {
		l_.emplace_back(gtn + 1, 0);
		l_[i][0] = i;
		for (auto j = 1U; j < gtn + 1; ++j) {
			l_[i][j] = getMin(i, j);
		}
	}
	backtrack();
	return l_[ocrn][gtn];
}

////////////////////////////////////////////////////////////////////////////////
size_t WagnerFischer::operator()(size_t b, size_t n) {
	assert((b + n) <= ocr_.size());
	// update gt: ...xxx...
	gt_ = ocr_.substr(0, b) + gt_ + ocr_.substr(b + n);
	return (*this)();
}

////////////////////////////////////////////////////////////////////////////////
size_t WagnerFischer::getMin(size_t i, size_t j) const noexcept {
	assert(i > 0);
	assert(j > 0);
	if (ocr_[i - 1] == gt_[j - 1]) {
		return l_[i - 1][j - 1];
	} else {
		return std::min(l_[i - 1][j - 1] + 1,
				std::min(l_[i - 1][j] + 1, l_[i][j - 1] + 1));
	}
}

////////////////////////////////////////////////////////////////////////////////
void WagnerFischer::backtrack() {
	trace_.clear();
	trace_.reserve(std::max(gt_.size(), ocr_.size()));
	for (size_t i = ocr_.size(), j = gt_.size(); i or j;) {
		auto t = backtrack(i, j);
		i = std::get<1>(t);
		j = std::get<2>(t);
		trace_.push_back(std::get<0>(t));
	}
	std::reverse(begin(trace_), end(trace_));
	for (size_t i = 0; i < trace_.size(); ++i) {
		switch (trace_[i]) {
			case EditOp::Nop:
			case EditOp::Sub:
				break;
			case EditOp::Del:
				ocr_.insert(i, 1, L'~');
				break;
			case EditOp::Ins:
				gt_.insert(i, 1, L'~');
				break;
		}
	}
	assert(trace_.size() == gt_.size());
	assert(trace_.size() == ocr_.size());
}

////////////////////////////////////////////////////////////////////////////////
std::tuple<WagnerFischer::EditOp, size_t, size_t> WagnerFischer::backtrack(
    size_t i, size_t j) const noexcept {
	if (i == 0) {
		assert(j != 0);
		return std::make_tuple(EditOp::Del, i, j - 1);
	} else if (j == 0) {
		assert(i != 0);
		return std::make_tuple(EditOp::Ins, i - 1, j);
	}

	assert(i > 0);
	assert(j > 0);
	const size_t x[] = {l_[i - 1][j - 1], l_[i][j - 1], l_[i - 1][j]};
	auto m = std::min_element(std::begin(x), std::end(x));
	switch (std::distance(std::begin(x), m)) {
		case 0:
			return x[0] == l_[i][j]
				   ? std::make_tuple(EditOp::Nop, i - 1, j - 1)
				   : std::make_tuple(EditOp::Sub, i - 1, j - 1);
		case 1:
			return std::make_tuple(EditOp::Del, i, j - 1);
		case 2:
			return std::make_tuple(EditOp::Ins, i - 1, j);
		default:
			assert(false);
	}
}

////////////////////////////////////////////////////////////////////////////////
std::pair<size_t, size_t> pcw::WagnerFischer::find_corrected_token(
    size_t i) const noexcept {
	assert(i <= trace_.size());  // it is ok if i points to end(trace_)
	const auto b = begin(trace_);
	const auto e = end(trace_);
	auto f = std::find_if_not(b + i, e,
				  [](auto op) { return op == EditOp::Nop; });
	if (f != e)
		return {find_begin_of_token(b, f), find_end_of_token(f, e)};
	else
		return {0, 0};
}

////////////////////////////////////////////////////////////////////////////////
size_t pcw::WagnerFischer::find_begin_of_token(Trace::const_iterator b,
					       Trace::const_iterator i) const
    noexcept {
	for (; i != b; --i) {
		if (*i == EditOp::Nop) {
			auto pos = std::distance(b, i);
			if (isspace(gt_[pos])) return pos + 1;
		}
	}
	assert(i == b);
	return gt_.empty() ? 0 : isspace(gt_[0]) ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
size_t pcw::WagnerFischer::find_end_of_token(Trace::const_iterator i,
					     Trace::const_iterator e) const
    noexcept {
	const auto b = begin(trace_);
	for (; i != e; ++i) {
		if (*i == EditOp::Nop) {
			auto pos = std::distance(b, i);
			if (isspace(gt_[pos])) return pos;
		}
	}
	return trace_.size();
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& pcw::operator<<(std::ostream& os,
			      const WagnerFischer::Trace& trace) {
	for (auto op : trace) os << static_cast<char>(op);
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& pcw::operator<<(std::ostream& os, const WagnerFischer& wf) {
	// os << wf.table() << "\n";

	char buf[] = {0, 0, 0, 0, 0};  // 5 are enough
	for (size_t i = 0; i < wf.ocr().size(); ++i) {
		utf8::append(wf.ocr()[i], buf);
		os << buf;
	}

	os << "\n";
	os << wf.trace();
	os << "\n";

	for (size_t i = 0; i < wf.gt().size(); ++i) {
		utf8::append(wf.gt()[i], buf);
		os << buf;
	}
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& pcw::operator<<(std::ostream& os, const WagnerFischer::Table& t) {
	for (const auto& row : t) {
		for (const auto i : row) {
			os << std::setw(3) << std::setfill(' ') << i;
		}
		os << "\n";
	}
	return os;
}
