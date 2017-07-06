#ifndef pcw_ParserPage_hpp__
#define pcw_ParserPage_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "core/Box.hpp"

namespace pcw {
enum class FileType;
class Page;
class Line;
class WagnerFischer;
class ParserChar;
class ParserPage;
class ParserLine;
using PagePtr = std::shared_ptr<Page>;
using ParserCharPtr = std::shared_ptr<ParserChar>;
using Path = boost::filesystem::path;
using ParserPagePtr = std::shared_ptr<ParserPage>;
using ParserLinePtr = std::shared_ptr<ParserLine>;
using LinePtr = std::shared_ptr<Line>;

class ParserLine {
       public:
	virtual ~ParserLine() noexcept = default;
	virtual void insert(size_t i, wchar_t c) = 0;
	virtual void erase(size_t i) = 0;
	virtual void set(size_t i, wchar_t c) = 0;
	virtual void noop(size_t) {}
	virtual void begin_wagner_fischer() {}
	virtual void end_wagner_fischer() {}
	virtual std::wstring wstring() const = 0;
	virtual std::string string() const = 0;
	virtual LinePtr line(int id) const = 0;
	size_t correct(WagnerFischer& wf);

	struct Char {
		Char(wchar_t cc, double cconf = 0, Box bbox = {})
		    : box(bbox), conf(cconf), c(cc) {}
		Box box;
		double conf;
		wchar_t c;
	};
	Box box;
};

class ParserPage : public std::enable_shared_from_this<ParserPage> {
       public:
	virtual ~ParserPage() noexcept = default;
	virtual void write(const Path&) const = 0;
	virtual ParserLine& get(size_t i) = 0;
	virtual const ParserLine& get(size_t i) const = 0;
	virtual size_t size() const noexcept = 0;
	virtual PagePtr page() const;

	Box box;
	Path ocr, img;
	FileType file_type;
	int id;
};
}

#endif  // pcw_ParserPage_hpp__
