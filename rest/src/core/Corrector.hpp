#ifndef pcw_Corrector_hpp__
#define pcw_Corrector_hpp__

#include <memory>
#include "WagnerFischer.hpp"

namespace pcw {
	class Line;
	class Project;
	using ProjectPtr = std::shared_ptr<Project>;

	class Corrector {
	public:
		Corrector(ProjectPtr view = nullptr)
			: wf_()
			, view_(std::move(view))
		{};
		void set_view(ProjectPtr view) noexcept {view_ = std::move(view);}
		void correct(int pid, int lid, const std::wstring& str);
		void correct(int pid, int lid, const std::string& str);


	private:
		void correct(Line& line, const std::wstring& gt, bool partial);

		WagnerFischer wf_;
		ProjectPtr view_;
	};
}

#endif // pcw_Corrector_hpp__
