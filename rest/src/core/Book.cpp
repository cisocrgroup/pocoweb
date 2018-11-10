#include "Book.hpp"
#include "Page.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
Book::set_owner(int owner)
{
  if (owner_)
    THROW(Error, "(Book) It is not allowed to change the owner of a book");
  owner_ = owner;
}
