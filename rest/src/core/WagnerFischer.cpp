#include <cwchar>
#include <cassert>
#include <algorithm>
#include <utf8.h>
#include "WagnerFischer.hpp"
#include "Line.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const std::wstring& truth, const Line& line)
{
	return (*this)(truth.data(), truth.size(), line);
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const wchar_t* truth, const Line& line)
{
	return (*this)(truth, wcslen(truth), line);
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const std::string& truth, const Line& line)
{
	return (*this)(truth.data(), truth.size(), line);
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const char* truth, const Line& line)
{
	std::wstring wstr;
	const auto n = strlen(truth);
	return (*this)(truth, n, line);
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const char* truth, size_t n, const Line& line)
{
	std::wstring wstr;
	wstr.reserve(n * 2);
	utf8::utf8to32(truth, truth + n, std::back_inserter(wstr));
	return (*this)(wstr.data(), wstr.size(), line);
}

////////////////////////////////////////////////////////////////////////////////
size_t 
WagnerFischer::operator()(const wchar_t* truth, size_t n, const Line& line)
{
        truth_ = std::wstring(truth, n);
        test_ = line.wstring();
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
pcw::WagnerFischer::getMin(size_t i, size_t j) const noexcept
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
pcw::WagnerFischer::backtrack()
{
        trace_.clear();
        trace_.reserve(std::max(truth_.size(), test_.size()));
        for (size_t i = test_.size(), j = truth_.size(); i and j; ) {
                auto t = backtrack(i, j);
                i = std::get<1>(t);
                j = std::get<2>(t);
                trace_.push_back(std::get<0>(t));
        }
        std::reverse(begin(trace_), end(trace_));
        for (size_t i = 0; i < trace_.size(); ++i) {
                switch (trace_[i].type) {
                case EditOp::Type::Nop:
                case EditOp::Type::Sub:
                        break;
                case EditOp::Type::Del:
                        test_.insert(i, 1, L'_');
                        break;
                case EditOp::Type::Ins:
                        truth_.insert(i, 1, L'_');
                        break;
                default:
                        assert(false);
                }
        }
        assert(trace_.size() == truth_.size());
        assert(trace_.size() == test_.size());
}

////////////////////////////////////////////////////////////////////////////////
std::tuple<EditOp, size_t, size_t>
pcw::WagnerFischer::backtrack(size_t i, size_t j) const noexcept
{
        assert(i > 0);
        assert(j > 0);
        const size_t x[] = {l_[i-1][j-1], l_[i][j-1], l_[i-1][j]};
        auto m = std::min_element(std::begin(x), std::end(x));
        switch (std::distance(std::begin(x), m)) {
        case 0:
                return x[0] == l_[i][j] ?
                        std::make_tuple(EditOp{EditOp::Type::Nop, truth_[j]}, i-1, j-1) :
                        std::make_tuple(EditOp{EditOp::Type::Sub, truth_[j]}, i-1, j-1);
        case 1:
                return std::make_tuple(EditOp{EditOp::Type::Del, truth_[j]}, i, j-1);
        case 2:
                return std::make_tuple(EditOp{EditOp::Type::Ins, truth_[j]}, i-1, j);
        default:
                assert(false);
        }
}

// ////////////////////////////////////////////////////////////////////////////////
// void
// pcw::WagnerFischer::applyTo(Line& line) const
// {
//         assert(trace_.size() == truth_.size());
//         assert(trace_.size() == test_.size());
//         int o = 0;
//         for (auto i = 0U; i < trace_.size(); ++i) {
//                 switch (trace_[i]) {
//                 case WagnerFischer::EditOp::S:
//                         line[i + o].setChar(truth_[i]);
//                         break;
//                 case WagnerFischer::EditOp::I:
//                         line.removeCharAt(i + o);
//                         --o;
//                         break;
//                 case WagnerFischer::EditOp::D:
//                         line.insertCharAt(i).setChar(truth_[i]);
//                         break;
//                 default:
//                         break;
//                 }
//         }
// }
