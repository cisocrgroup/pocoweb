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
	return database(session());
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<Database> 
Route::database(SessionPtr session) const noexcept
{
	assert(config_);
	return session ? Database(session, config_) : boost::optional<Database>{};
}
