#include <utf8.h>
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"
#include "Corrector.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
Corrector::correct(int pid, int lid, const std::wstring& str)
{
	if (view_) {
		auto page = view_->find(pid);
		if (page)
			correct((*page)[lid], str, false);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Corrector::correct(int pid, int lid, const std::string& str)
{
	std::wstring wstr;
	wstr.reserve(str.size() * 2);
	utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
	correct(pid, lid, wstr);
}

////////////////////////////////////////////////////////////////////////////////
void
Corrector::correct(Line& line, const std::wstring& gt, bool partial)
{
	wf_.set_ocr(line);
	wf_.set_gt(gt);
	wf_();
	wf_.correct(line, partial);
}
