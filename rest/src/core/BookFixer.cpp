#include <algorithm>
#include <iostream>
#include "BadRequest.hpp"
#include "Book.hpp"
#include "Page.hpp"
#include "BookFixer.hpp"

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
BookFixer::BookFixer(Paths imgs)
	: imgs_(std::move(imgs))
{
}

////////////////////////////////////////////////////////////////////////////////
void
BookFixer::fix(Book& book) const
{
	// remove all null pages
	std::remove_if(begin(book), end(book), [](const auto& page) {
		return not page;
	});
	fix_page_and_line_ordering(book);
	fix_image_paths(book);
}

////////////////////////////////////////////////////////////////////////////////
void
BookFixer::fix_page_and_line_ordering(Book& book) const
{
	const auto b = begin(book);
	const auto e = end(book);

	if (std::all_of(b, e, [](const auto& page) {return page->id() > 0;})) {
		// sort by page index of ocr source file
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->id() < b->id();
		});
	} else if (std::any_of(b, e, [](const auto& page) {return page->id() > 0;})) {
		// sort by page index AND path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			if (a->id() > 0 and b->id() > 0) {
				return a->id() < b->id();
			} else {
				return a->ocr.stem() < b->ocr.stem();
			}
		});
		update_indizes(book);
	} else {
		// sort by path stem
		std::sort(b, e, [](const auto& a, const auto& b) {
			return a->ocr.stem() < b->ocr.stem();
		});
		update_indizes(book);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookFixer::update_indizes(Book& book) const
{
	int id = 0;
	for (auto& page: book) {
		page->set_id(++id);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookFixer::fix_image_paths(Book& book) const
{
	for (const auto& page: book) {
		fix_image_paths(*page);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
BookFixer::fix_image_paths(Page& page) const
{
	const auto b = begin(imgs_);
	const auto e = end(imgs_);

	// find by the reference in the ocr source file
	auto i = std::find_if(b, e, [&page](const auto& path) {
		return path.filename() == page.img.filename();
	});
	if (i != e) {
		page.img = *i;
		return;
	}

	// find by matching file names
	i = std::find_if(b, e, [&page](const auto& path) {
		return path.stem() == page.ocr.stem();
	});
	if (i != e) {
		page.img = *i;
		return;
	}

	// cannot find the image file
	throw BadRequest(
		"(BookFixer) Cannot find image file for " +
		page.ocr.string() + " (" + page.img.string() + ")"
	);
}

