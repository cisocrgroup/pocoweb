#include "PageBuilder.hpp"
#include "Box.hpp"
#include "Line.hpp"
#include "Page.hpp"
#include <cassert>
#include <cstring>
#include <cwchar>
#include <utf8.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
PageBuilder&
PageBuilder::reset()
{
  page_ = std::make_shared<Page>();
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::append(Line& line) const
{
  assert(page_);
  // invalid id; assume line should be appended
  if (line.id() == 0) {
    page_->push_back(nullptr);
    line.id_ = static_cast<int>(page_->size());
  } else { // grow page (insert nullptr for deleted / missing line ids)
    while (static_cast<int>(page_->size()) != line.id()) {
      page_->push_back(nullptr);
    }
  }
  // insert line at line.id() - 1
  auto newLine = line.shared_from_this();
  newLine->page_ = page_;
  page_->insert_line(newLine);
  assert(not page_->empty());
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::set_id(int pageid) const
{
  assert(page_);
  page_->set_id(pageid);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::set_image_path(Path image) const
{
  assert(page_);
  page_->img = std::move(image);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::set_ocr_path(Path ocr) const
{
  assert(page_);
  page_->ocr = std::move(ocr);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::set_box(Box box) const
{
  assert(page_);
  page_->box = box;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
const PageBuilder&
PageBuilder::set_file_type(FileType filetype) const
{
  assert(page_);
  page_->file_type = filetype;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
PageSptr
PageBuilder::build() const
{
  assert(page_);
  return page_;
}
