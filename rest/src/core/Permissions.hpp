#ifndef pcw_Permissions_hpp__
#define pcw_Permissions_hpp__

#include <sqlpp11/sqlpp11.h>
#include "database/Database.hpp"
#include "database/Tables.h"

namespace pcw {

namespace detail {
template <class Db>
bool all_projects_are_finished(Db& db, int userid, int pid);
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
		    Permissions perm);
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
bool pcw::detail::all_projects_are_finished(Db& db, int userid, int bookid) {
	using namespace sqlpp;
	tables::Projects p;
	auto pids = db(select(p.owner).from(p).where(p.origin == bookid));
	for (const auto& pid : pids) {
		if (static_cast<int>(pid.owner) != userid) return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
template <class Db>
bool pcw::has_permission(Db& db, int userid, bool admin, int projectid,
			 Permissions perm) {
	// Only admins can create new projects (ignore project id).
	if (perm == Permissions::Create) return admin;
	const auto entry = pcw::select_project_entry(db, projectid);
	// If project is not found or the user is not the owner of the project,
	// he has no access.
	if (not entry or entry->owner != userid) return false;
	switch (perm) {
		// Admins and normal users can read and write
		// (if they own it).
		case Permissions::Read:
		case Permissions::Write:
			return true;
		// Admins and normal users can finish projects but not books.
		case Permissions::Finish:
			return not entry->is_book();
		// Only admins can remove books and projects
		case Permissions::Remove:
			if (not admin) return false;
			if (entry->is_book())
				return detail::all_projects_are_finished(
				    db, userid, projectid);
			else
				return true;
		// Only admins can split books (and not projects)
		case Permissions::Split:
			return admin and entry->is_book();
		// Only admins can assign project (and not books).
		case Permissions::Assign:
			return admin and not entry->is_book();
		// Only admins can create projects
		default:
			return admin;
	}
}

#endif  // pcw_Permissions_hpp__
