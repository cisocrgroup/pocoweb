#include <cwchar>
#include <utf8.h>
#include <cassert>
#include <cstring>
#include "Line.hpp"
#include "Page.hpp"
#include "Box.hpp"
#include "PageBuilder.hpp"

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
	page_->push_back(line.shared_from_this());
	assert(not page_->empty());
	page_->back()->id_ = static_cast<int>(page_->size());
	page_->back()->page_ = page_;
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
PageSptr
PageBuilder::build() const
{
	assert(page_);
	return page_;
}
