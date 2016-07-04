#ifndef pcw_BookData_hpp__
#define pcw_BookData_hpp__

namespace pcw {
	struct BookData {
		std::string title, author, desc, uri, path;
		int id, year, firstpage, lastpage;
	};
}

#endif // pcw_BookData_hpp__
