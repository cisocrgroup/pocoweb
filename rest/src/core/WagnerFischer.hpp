#ifndef pcw_WagnerFischer_hpp__
#define pcw_WagnerFischer_hpp__

#include <vector>
#include <string>
#include <tuple>

namespace pcw {
        class Line;

	struct EditOp {
		enum class Type {Del='-', Ins='+', Sub='#', Nop='|'};
		Type type;
		wchar_t letter;
	};

        class WagnerFischer {
        public:
                using Trace = std::vector<EditOp>;

                size_t operator()(const std::wstring& truth, const Line& line);
                size_t operator()(const wchar_t* truth, const Line& line);
                size_t operator()(const wchar_t* truth, size_t n, const Line& line);
                size_t operator()(const std::string& truth, const Line& line);
                size_t operator()(const char* truth, const Line& line);
                size_t operator()(const char* truth, size_t n, const Line& line);

                const std::wstring& getTest() const noexcept {
                        return test_;
                }
                const std::wstring& getTruth() const noexcept {
                        return truth_;
                }
                const Trace& getTrace() const noexcept {
                        return trace_;
                }

        private:
                size_t getMin(size_t i, size_t j) const noexcept;
                void backtrack();
                std::tuple<EditOp, size_t, size_t>
                backtrack(size_t i, size_t j) const noexcept;

                using Table = std::vector<std::vector<size_t> >;
                std::wstring truth_, test_;
                Trace trace_;
                Table l_;
        };
};

#endif // pcw_WagnerFischer_hpp__
