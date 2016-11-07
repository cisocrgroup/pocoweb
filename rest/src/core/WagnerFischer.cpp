#include <cwchar>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <utf8.h>
#include <crow/logging.h>
#include "WagnerFischer.hpp"
#include "Line.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const std::string& truth)
{
	set_truth(truth.data(), truth.size());
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const char* truth)
{
	set_truth(truth, strlen(truth));
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const char* truth, size_t n)
{
	std::wstring wstr;
	wstr.reserve(n * 2);
	utf8::utf8to32(truth, truth + n, std::back_inserter(wstr));
	set_truth(wstr.data(), wstr.size());
}
	
////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const std::wstring& truth)
{
	truth_ = truth;
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const wchar_t* truth)
{
	set_truth(truth, wcslen(truth));
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const wchar_t* truth, size_t n)
{
	truth_ = std::wstring(truth, n);
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_truth(const Line& line)
{
	truth_ = line.wstring();
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const std::string& test)
{
	set_test(test.data(), test.size());
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const char* test)
{
	set_test(test, strlen(test));
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const char* test, size_t n)
{
	std::wstring wstr;
	wstr.reserve(n * 2);
	utf8::utf8to32(test, test + n, std::back_inserter(wstr));
	set_test(wstr.data(), wstr.size());
}
	
////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const std::wstring& test)
{
	test_ = test;
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const wchar_t* test)
{
	set_test(test, wcslen(test));
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const wchar_t* test, size_t n)
{
	test_ = std::wstring(test, n);
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::set_test(const Line& line)
{
	test_ = line.wstring();
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()()
{
        const auto truthn = truth_.size();
        const auto testn = test_.size();
        l_.clear();
        l_.emplace_back(truthn + 1, 0);
        std::iota(l_[0].begin(), l_[0].end(), 0);
        for (auto i = 1U; i < testn + 1; ++i) {
                l_.emplace_back(truthn + 1, 0);
                l_[i][0] = i;
                for (auto j = 1U; j < truthn + 1; ++j) {
                        l_[i][j] = getMin(i, j);
                }
        }
        backtrack();
        return l_[testn][truthn];
}

////////////////////////////////////////////////////////////////////////////////
size_t
WagnerFischer::getMin(size_t i, size_t j) const noexcept
{
        assert(i > 0);
        assert(j > 0);
        if (test_[i - 1] == truth_[j - 1]) {
                return l_[i - 1][j - 1];
        } else {
                return std::min(l_[i - 1][j - 1] + 1,
                                std::min(l_[i - 1][j] + 1, l_[i][j - 1] + 1));

        }
}

////////////////////////////////////////////////////////////////////////////////
void
WagnerFischer::backtrack()
{
        trace_.clear();
        trace_.reserve(std::max(truth_.size(), test_.size()));
        for (size_t i = test_.size(), j = truth_.size(); i or j;) {
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
                        test_.insert(i, 1, L'_');
                        break;
                case EditOp::Ins:
                        truth_.insert(i, 1, L'_');
                        break;
                }
        }
        assert(trace_.size() == truth_.size());
        assert(trace_.size() == test_.size());
}

////////////////////////////////////////////////////////////////////////////////
std::tuple<WagnerFischer::EditOp, size_t, size_t>
WagnerFischer::backtrack(size_t i, size_t j) const noexcept
{
	if (i == 0) {
		assert(j != 0);
		return std::make_tuple(EditOp::Del, i, j-1);
	} else if (j == 0) {
		assert(i != 0);
                return std::make_tuple(EditOp::Ins, i-1, j);
	}
		
        assert(i > 0);
        assert(j > 0);
        const size_t x[] = {l_[i-1][j-1], l_[i][j-1], l_[i-1][j]};
        auto m = std::min_element(std::begin(x), std::end(x));
        switch (std::distance(std::begin(x), m)) {
        case 0:
                return x[0] == l_[i][j] ?
                        std::make_tuple(EditOp::Nop, i-1, j-1) :
                        std::make_tuple(EditOp::Sub, i-1, j-1);
        case 1:
                return std::make_tuple(EditOp::Del, i, j-1);
        case 2:
                return std::make_tuple(EditOp::Ins, i-1, j);
        default:
                assert(false);
        }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& 
pcw::operator<<(std::ostream& os, const WagnerFischer& wf)
{
	os << wf.table() << "\n";

	char buf[] = {0,0,0,0,0}; // 5 are enough
	for (size_t i = 0; i < wf.test().size(); ++i) {
		utf8::append(wf.test()[i], buf);
		os << buf;
	}
	os << "\n";
	for (size_t i = 0; i < wf.trace().size(); ++i) {
		os << static_cast<char>(wf.trace()[i]);
	}
	os << "\n";
	for (size_t i = 0; i < wf.truth().size(); ++i) {
		utf8::append(wf.truth()[i], buf);
		os << buf;
	}	
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& 
pcw::operator<<(std::ostream& os, const WagnerFischer::Table& t)
{
	for (const auto& row: t) {
		for (const auto i: row) {
			os << std::setw(3) << std::setfill(' ') << i;
		}
		os << "\n";
	}
	return os;
}

