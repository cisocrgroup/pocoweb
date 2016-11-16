#include <iostream>
#include <fstream>
#include <utf8.h>
#include "core/Page.hpp"
#include "core/WagnerFischer.hpp"
#include "ParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
size_t 
ParserLine::correct(WagnerFischer& wf)
{
	wf.set_ocr(wstring());
	return wf.apply(*this);	
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
ParserPage::page() const
{
	auto page = std::make_shared<Page>(id, box);
	page->ocr = ocr;
	page->img = img;
	const auto n = size();
	for (auto i = 0U; i < n; ++i) {
		auto id = static_cast<int>(i + 1);
		page->push_back(get(i).line(id));
	}
	return page;
}
