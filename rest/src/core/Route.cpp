#include "Database.hpp"
#include "Route.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
SessionPtr
Route::session() const noexcept
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Database> 
Route::database() const noexcept
{
	assert(config_);
	auto s = session();
	return s ? Database(s, config_) : boost::optional<Database>{};
}
