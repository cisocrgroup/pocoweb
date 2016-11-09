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
	using ParserCharPtr = std::shared_ptr<ParserChar>;	
	using Path = boost::filesystem::path;
	using ParserPagePtr = std::shared_ptr<ParserPage>;

	struct ParserLine {
		Box box;
		std::vector<ParserCharPtr> chars;
		std::wstring wstring() const noexcept; 
		std::string string() const noexcept; 

		// interface for WagnerFischer correction
		void insert(size_t i, wchar_t c);
		void erase(size_t i);
		void set(size_t i, wchar_t c);
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
