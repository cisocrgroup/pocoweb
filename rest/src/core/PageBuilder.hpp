#ifndef pcw_PageBuilder_hpp__
#define pcw_PageBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Box;
class Line;
class Page;
using PageSptr = std::shared_ptr<Page>;
using Path = boost::filesystem::path;
enum class FileType;

class PageBuilder {
       public:
	PageBuilder() : page_() { reset(); }

	PageBuilder& reset();
	const PageBuilder& append(Line& line) const;
	const PageBuilder& set_id(int pageid) const;
	const PageBuilder& set_image_path(Path img) const;
	const PageBuilder& set_ocr_path(Path ocr) const;
	const PageBuilder& set_box(Box box) const;
	const PageBuilder& set_file_type(FileType filetype) const;
	PageSptr build() const;

       private:
	PageSptr page_;
};
}

#endif  // pcw_LineBuilder_hpp__
