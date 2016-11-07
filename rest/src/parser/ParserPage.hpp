#ifndef pcw_ParserPage_hpp__
#define pcw_ParserPage_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>
#include <vector>
#include "core/Box.hpp"

namespace pcw {
	class WagnerFischer;
	class ParserChar;
	using ParserCharPtr = std::shared_ptr<ParserChar>;	
	using Path = boost::filesystem::path;

	struct ParserLine {
		Box box;
		std::vector<ParserCharPtr> chars;
		std::wstring wstring() const noexcept; 

		// interface for WagnerFischer correction
		void insert(size_t i, wchar_t c);
		void erase(size_t i);
		void set(size_t i, wchar_t c);
	};	

	struct ParserPage {
		Box box;
		Path ocr, img;
		std::vector<ParserLine> lines;
		int id;
		void write(const Path& path) const;
	};
}
#endif // pcw_ParserPage_hpp__
