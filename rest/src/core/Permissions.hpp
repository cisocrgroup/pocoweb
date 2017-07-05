#ifndef pcw_Permissions_hpp__
#define pcw_Permissions_hpp__

#include "database/Tables.h"
namespace pcw {

namespace detail {

template <class Db>
int get_owner_id(int projectid);
}

enum class Permissions {
	Read,
	Write,
	Assign,
	Remove,
	Finish,
	Create,
	Split,
};

template <class Db>
bool has_permission(Db& db, int userid, bool admin, int projectid,
		    Permission perm);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
int pcw::detail::get_owner_id(int projectid) {
	using namespace sqlpp;
	tables::Projects p;
	const auto res =
	    db(select(p.owner).from(p).where(p.projectid == projectid));
	if (not res.empty())
		return res.front().owner;
	else
		return -1;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
bool pcw::has_permission(Db& db, int userid, bool admin, int projectid,
			 Permission perm) {
	// Only admins can create new projects (ignore project id).
	if (perm == Permissions::Create) return admin;
	const auto ownerid = detail::get_owner_id(projectid);
	// If project is not found or the user is not the owner of the project,
	// he has no access.
	if (ownerid <= 0 or ownerid != userid) return false;
	switch (Perm) {
		// Admins and normal users can read, write and finish projects
		// (if they own it).
		case Permissions::Read:
		case Permissions::Write:
		case Permissions::Finish:
			return true;
		// Only admins can remove, split or assign projects.
		default:
			return admin;
	}
}

#endif  // pcw_Permissions_hpp__
