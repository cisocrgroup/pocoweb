#include <cassert>
#include "Book.hpp"
#include "User.hpp"
#include "Page.hpp"
#include "Package.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Package::Package(int id, const User& owner, const Book& book)
	: BookView(id)
	, owner_(owner.shared_from_this())
	, origin_(std::static_pointer_cast<const Book>(book.shared_from_this()))
{
}

////////////////////////////////////////////////////////////////////////////////
Package::Package(int id)
	: BookView(id)
	, owner_()
	, origin_()
{
}

////////////////////////////////////////////////////////////////////////////////
const Book&
Package::origin() const noexcept
{
	return *origin_;
}

////////////////////////////////////////////////////////////////////////////////
const User&
Package::owner() const noexcept
{
	return *owner_;
}

////////////////////////////////////////////////////////////////////////////////
void
Package::set_origin(const Book& book)
{
	origin_ = std::static_pointer_cast<const Book>(book.shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
void
Package::set_owner(const User& owner)
{
	owner_ = owner.shared_from_this();
}
