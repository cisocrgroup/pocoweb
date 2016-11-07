#ifndef pcw_WagnerFischer_hpp__
#define pcw_WagnerFischer_hpp__

#include <vector>
#include <string>
#include <tuple>

namespace pcw {
        class Line;

        class WagnerFischer {
        public:
		enum class EditOp: char {Del='-', Ins='+', Sub='#', Nop='|'};
                using Trace = std::vector<EditOp>;
                using Table = std::vector<std::vector<size_t> >;

		void set_test(const std::string& test);
		void set_test(const char* test);
		void set_test(const char* test, size_t n);
		void set_test(const std::wstring& test);
		void set_test(const wchar_t* test);
		void set_test(const wchar_t* test, size_t n);
		void set_test(const Line& line);

		void set_truth(const std::string& truth);
		void set_truth(const char* truth);
		void set_truth(const char* truth, size_t n);
		void set_truth(const std::wstring& truth);
		void set_truth(const wchar_t* truth);
		void set_truth(const wchar_t* truth, size_t n);
		void set_truth(const Line& line);

                size_t operator()();
	
                const std::wstring& test() const noexcept {
                        return test_;
                }
                const std::wstring& truth() const noexcept {
                        return truth_;
                }
                const Trace& trace() const noexcept {
                        return trace_;
                }
		const Table& table() const noexcept { // just for debugging
			return l_;
		}
		template<class T>
		void apply(T& t) const;

        private:
                size_t getMin(size_t i, size_t j) const noexcept;
                void backtrack();
                std::tuple<EditOp, size_t, size_t>
                backtrack(size_t i, size_t j) const noexcept;

                std::wstring truth_, test_;
                Trace trace_;
                Table l_;
        };
	std::ostream& operator<<(std::ostream& os, const WagnerFischer& wf);
	std::ostream& operator<<(std::ostream& os, const WagnerFischer::Table& t);
};

////////////////////////////////////////////////////////////////////////////////
template<class T>
void 
pcw::WagnerFischer::apply(T& t) const
{
	int o = 0;
	const auto n = trace_.size();
	for (size_t i = 0; i < n; ++i) {
		const auto ii = i + o;
		switch (trace_[i]) {
		// insertion means that the ocr recognized a character
		// where in reality there should be none; delete it
		case WagnerFischer::EditOp::Ins:
			t.erase(ii);
			--o;	
			break;
		// deletion means that the ocr did not recognize a character 
		// where in reality there should be one; so insert it
		case WagnerFischer::EditOp::Del: 
			t.insert(ii, truth()[i]);
			break;
		// subustitution just updates the according char
		case WagnerFischer::EditOp::Sub:
			t.set(ii, truth()[i]);
			break;
		// do nothing; char is already correct
		case WagnerFischer::EditOp::Nop:
			break;
		}
	}
}

#endif // pcw_WagnerFischer_hpp__
