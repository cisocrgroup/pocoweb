#ifndef pcw_WagnerFischer_hpp__
#define pcw_WagnerFischer_hpp__

#include <regex>
#include <string>
#include <tuple>
#include <vector>

namespace pcw {
class Line;

class WagnerFischer {
       public:
	enum class EditOp : char { Del = '-', Ins = '+', Sub = '#', Nop = '|' };
	using Trace = std::vector<EditOp>;
	using Table = std::vector<std::vector<size_t> >;

	void set_ocr(const std::string& ocr);
	void set_ocr(const char* ocr);
	void set_ocr(const char* ocr, size_t n);
	void set_ocr(const std::wstring& ocr);
	void set_ocr(const wchar_t* ocr);
	void set_ocr(const wchar_t* ocr, size_t n);
	void set_ocr(const Line& line);

	void set_gt(const std::string& gt);
	void set_gt(const char* gt);
	void set_gt(const char* gt, size_t n);
	void set_gt(const std::wstring& gt);
	void set_gt(const wchar_t* gt);
	void set_gt(const wchar_t* gt, size_t n);
	void set_gt(const Line& line);
	void set_gt(const std::wregex& pat, const std::wstring& repl);

	void clear() {
		l_.clear();
		trace_.clear(), gt_.clear();
		ocr_.clear();
	}

	size_t operator()();
	size_t operator()(size_t b, size_t n);

	const std::wstring& ocr() const noexcept { return ocr_; }
	const std::wstring& gt() const noexcept { return gt_; }
	const Trace& trace() const noexcept { return trace_; }
	const Table& table() const noexcept { return l_; }
	template <class T>
	void correct(T& t, size_t b, size_t n) const;
	template <class T>
	void correct(T& t, bool partial = false) const;

       private:
	template <class T>
	void correct_impl(size_t b, size_t n, T& t) const;
	template <class T>
	void correct_at(size_t i, int& ofs, T& t) const;
	std::pair<size_t, size_t> find_corrected_token(size_t i) const noexcept;
	size_t find_begin_of_token(Trace::const_iterator b,
				   Trace::const_iterator i) const noexcept;
	size_t find_end_of_token(Trace::const_iterator i,
				 Trace::const_iterator e) const noexcept;
	size_t getMin(size_t i, size_t j) const noexcept;
	void backtrack();
	std::tuple<EditOp, size_t, size_t> backtrack(size_t i, size_t j) const
	    noexcept;

	std::wstring gt_, ocr_;
	Trace trace_;
	Table l_;
};
std::ostream& operator<<(std::ostream& os, const WagnerFischer& wf);
std::ostream& operator<<(std::ostream& os, const WagnerFischer::Trace& trace);
std::ostream& operator<<(std::ostream& os, const WagnerFischer::Table& t);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void pcw::WagnerFischer::correct(T& t, size_t b, size_t n) const {
	correct_impl(b, n, t);
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void pcw::WagnerFischer::correct(T& t, bool partial) const {
	if (not partial) {
		correct_impl(0, trace_.size(), t);
	} else {
		for (auto r = find_corrected_token(0);
		     r.first != 0 or r.second != 0;
		     r = find_corrected_token(r.second)) {
			correct_impl(r.first, r.second, t);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
void pcw::WagnerFischer::correct_impl(size_t b, size_t e, T& t) const {
	int ofs = 0;
	t.begin_wagner_fischer();
	for (size_t i = b; i != e; ++i) {
		correct_at(i, ofs, t);
	}
	t.end_wagner_fischer();
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
inline void pcw::WagnerFischer::correct_at(size_t i, int& ofs, T& t) const {
	const auto ii = i + ofs;
	switch (trace_[i]) {
		// insertion means that the ocr recognized a character
		// where in reality there should be none; delete it
		case WagnerFischer::EditOp::Ins:
			t.erase(ii);
			--ofs;
			break;
		// deletion means that the ocr did not recognize a
		// character
		// where in reality there should be one; so insert it
		case WagnerFischer::EditOp::Del:
			t.insert(ii, gt_[i]);
			break;
		// subustitution just updates the according char
		case WagnerFischer::EditOp::Sub:
			t.set(ii, gt_[i]);
			break;
		// do nothing; char is already correct
		case WagnerFischer::EditOp::Nop:
			t.noop(ii);
			break;
	}
}

#endif  // pcw_WagnerFischer_hpp__
