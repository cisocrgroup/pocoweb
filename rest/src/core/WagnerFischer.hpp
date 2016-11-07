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

#endif // pcw_WagnerFischer_hpp__
