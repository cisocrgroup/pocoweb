#include <iostream>
#include <fstream>
#include <utf8.h>
#include "core/WagnerFischer.hpp"
#include "ParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
size_t 
ParserLine::correct(WagnerFischer& wf)
{
	wf.set_test(wstring());
	begin_correction();
	auto lev = wf();
	wf.apply(*this);	
	end_correction();
	return lev;
}
