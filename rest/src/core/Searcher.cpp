#include "Project.hpp"
#include "Searcher.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Searcher::Searcher(Project& project): Searcher(project.shared_from_this()) {}

////////////////////////////////////////////////////////////////////////////////
void
Searcher::set_project(Project& project)
{
	project_ = project.shared_from_this();
}
