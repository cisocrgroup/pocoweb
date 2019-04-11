#include "Page.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
Page::delete_line(int id)
{
  if (id <= 0 or id > static_cast<int>(this->size())) {
    return;
  }
  Base::operator[](id - 1) = nullptr;
  // (*this)[id - 1] = nullptr;
  // Base::erase(this->begin() + (id - 1));
}
