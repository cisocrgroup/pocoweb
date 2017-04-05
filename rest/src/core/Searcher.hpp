#ifndef pcw_Searcher_hpp__
#define pcw_Searcher_hpp__

#include <memory>
#include <vector>
#include "Project.hpp"
#include "Page.hpp"
#include "Line.hpp"

namespace pcw {
	using LineSptr = std::shared_ptr<Line>;
	using ProjectSptr = std::shared_ptr<Project>;

	class Searcher {
	public:
		Searcher(): Searcher(nullptr) {}
		Searcher(Project& project);

		void set_project(Project& project) noexcept;
		bool match_words() const noexcept
		{
			return match_words_;
		}
		void set_match_words(bool m=true) noexcept
		{
			match_words_ = m;
		}
		bool ignore_case() const noexcept
		{
			return ignore_case_;
		}
		void set_ignore_case(bool i=true) noexcept
		{
			ignore_case_ = i;
		}

		std::vector<LineSptr> find(const std::wstring& str) const;
		std::vector<LineSptr> find(const std::string& str) const;

		template<class F>
		std::vector<LineSptr> find_impl(F f) const;

	private:
		Searcher(ProjectSptr p)
			: project_(std::move(p))
		  	, match_words_(true)
		  	, ignore_case_(true)
		{}
		ProjectSptr project_;
		bool match_words_, ignore_case_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
inline std::vector<pcw::LineSptr>
pcw::Searcher::find_impl(F f) const
{
	if (not project_)
		return {};
	std::vector<pcw::LineSptr> res;
	for (const auto& page: *project_) {
		if (page) {
			for (const auto& line: *page) {
				if (line and f(*line))
					res.push_back(line);
			}
		}
	}
	return res;
}

#endif // pcw_Searcher_hpp__
