#ifndef pcw_ParserPage_hpp__
#define pcw_ParserPage_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "core/Box.hpp"

namespace pcw {
	class WagnerFischer;
	class ParserChar;
	class ParserPage;
	class ParserLine;
	using ParserCharPtr = std::shared_ptr<ParserChar>;	
	using Path = boost::filesystem::path;
	using ParserPagePtr = std::shared_ptr<ParserPage>;
	using ParserLinePtr = std::shared_ptr<ParserLine>;

	class ParserLine {
	public:
		virtual ~ParserLine() noexcept = default;
		virtual void insert(size_t i, wchar_t c) = 0;
		virtual void erase(size_t i) = 0;
		virtual void set(size_t i, wchar_t c) = 0;
		virtual void begin_correction() {}
		virtual void end_correction() {}
		virtual std::wstring wstring() const = 0;
		virtual std::string string() const = 0;
		size_t correct(WagnerFischer& wf);

		Box box;
	};	

	class ParserPage {
	public:
		virtual ~ParserPage() noexcept = default;
		virtual void write(const Path&) const = 0;
		virtual ParserLine& get(size_t i) = 0;
		virtual size_t size() const noexcept = 0;
		Box box;
		Path ocr, img;
		int id;
	};
}

#endif // pcw_ParserPage_hpp__
