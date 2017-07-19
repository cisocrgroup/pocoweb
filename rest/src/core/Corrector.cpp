#include "Corrector.hpp"
#include <utf8.h>
#include "Line.hpp"
#include "Page.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, int lid, const std::wstring& str) {
	if (view_) {
		auto page = view_->find(pid);
		if (page) correct((*page)[lid], str, false);
	}
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, int lid, const std::string& str) {
	std::wstring wstr;
	wstr.reserve(str.size() * 2);
	utf8::utf8to32(begin(str), end(str), std::back_inserter(wstr));
	correct(pid, lid, wstr);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, int lid, const std::wregex& pat,
			const std::wstring repl) {
	if (view_) {
		auto page = view_->find(pid);
		if (page) {
			correct((*page)[lid], pat, repl);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, int lid, const std::wstring& pat,
			const std::wstring repl) {
	correct(pid, lid, std::wregex{pat}, repl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, int lid, const std::string& pat,
			const std::string repl) {
	std::wstring wpat, wrepl;
	wpat.reserve(pat.size() * 2);
	wrepl.reserve(repl.size() * 2);
	utf8::utf8to32(begin(pat), end(pat), std::back_inserter(wpat));
	utf8::utf8to32(begin(repl), end(repl), std::back_inserter(wrepl));
	correct(pid, lid, std::wregex{wpat}, wrepl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, const std::wregex& pat,
			const std::wstring repl) {
	if (view_) {
		auto page = view_->find(pid);
		if (page) {
			for (auto& line : *page) correct(*line, pat, repl);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, const std::wstring& pat,
			const std::wstring repl) {
	correct(pid, std::wregex{pat}, repl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(int pid, const std::string& pat,
			const std::string repl) {
	std::wstring wpat, wrepl;
	wpat.reserve(pat.size() * 2);
	wrepl.reserve(repl.size() * 2);
	utf8::utf8to32(begin(pat), end(pat), std::back_inserter(wpat));
	utf8::utf8to32(begin(repl), end(repl), std::back_inserter(wrepl));
	correct(pid, std::wregex{wpat}, wrepl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(const std::wregex& pat, const std::wstring repl) {
	if (view_) {
		for (const auto& page : *view_) {
			if (page) {
				for (auto& line : *page)
					correct(*line, pat, repl);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(const std::wstring& pat, const std::wstring repl) {
	correct(std::wregex{pat}, repl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(const std::string& pat, const std::string repl) {
	std::wstring wpat, wrepl;
	wpat.reserve(pat.size() * 2);
	wrepl.reserve(repl.size() * 2);
	utf8::utf8to32(begin(pat), end(pat), std::back_inserter(wpat));
	utf8::utf8to32(begin(repl), end(repl), std::back_inserter(wrepl));
	correct(std::wregex{wpat}, wrepl);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(Line& line, const std::wstring& gt, bool partial) {
	wf_.set_ocr(line);
	wf_.set_gt(gt);
	wf_();
	wf_.correct(line, partial);
}

////////////////////////////////////////////////////////////////////////////////
void Corrector::correct(Line& line, const std::wregex& pat,
			const std::wstring& repl) {
	wf_.clear();
	wf_.set_ocr(line);
	wf_.set_gt(pat, repl);
	wf_();
	wf_.correct(line, true);
}
