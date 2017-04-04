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
		void set_project(Project& project);

		template<class F>
		std::vector<LineSptr> find(F f) const;

	private:
		Searcher(ProjectSptr p)
			: project_(std::move(p))
		{}
		ProjectSptr project_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
inline std::vector<pcw::LineSptr>
pcw::Searcher::find(F f) const
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
