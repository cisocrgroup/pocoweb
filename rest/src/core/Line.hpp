#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include <memory>
#include <vector>
#include "Box.hpp"

namespace pcw {
	class Page;
	using PagePtr = std::shared_ptr<Page>;

	class Line {
	public:
		using String = std::string;
		using Cuts = std::vector<int>;
	
		Line(Page& page, int i, Box b = {});
	
		bool empty() const noexcept {return string_.empty();}
		size_t size() const noexcept {return string_.size();}
		const String& string() const noexcept {return string_;}
		const Cuts& cuts() const noexcept {return cuts_;}

		void append(const char* str, const Box& box);
		void append(char c, const Box& box);

		const std::weak_ptr<Page> page;
		const Box box;
		const int id;

	private:
		String string_;
		Cuts cuts_;
	};
}

#endif // pcw_Line_hpp__
