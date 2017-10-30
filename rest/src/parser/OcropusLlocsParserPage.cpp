#include "OcropusLlocsParserPage.hpp"
#include <crow/logging.h>
#include <utf8.h>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "core/Line.hpp"
#include "llocs.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
OcropusLlocsParserPage::OcropusLlocsParserPage(int iid) : dir_() {
	this->id = iid;
	std::stringstream dir;
	dir << std::hex << std::setfill('0') << std::setw(10) << id;
	dir_ = dir.str();
}

////////////////////////////////////////////////////////////////////////////////
void OcropusLlocsParserPage::write(const Path& path) const {
	boost::filesystem::create_directories(path);
	int i = 0;
	for (const auto& line : lines()) {
		write(++i, *line, path);
	}
}

////////////////////////////////////////////////////////////////////////////////
void OcropusLlocsParserPage::write(int id, const ParserLine& line,
				   const Path& base) {
	const auto l = line.line(id);
	const auto llocs = get_llocs_from_png(base / l->img.filename());
	std::ofstream os(llocs.string());
	if (not os.good()) {
		throw std::system_error(errno, std::system_category(),
					llocs.string());
	}
	size_t nchars = 0;
	for (const auto& c : l->chars()) {
		const auto cc = c.get_cor();
		if (cc) {
			char buffer[10];
			auto x = utf8::utf32to8(&cc, &cc + 1, buffer);
			*x = 0;

			os << buffer << '\t' << static_cast<double>(c.cut)
			   << '\t' << c.conf << std::endl;
			if (not os.good()) {
				THROW(Error,
				      "(OcropusLlocsParserPage) Could not "
				      "write file ",
				      llocs.string(), ": ",
				      std::error_code(errno,
						      std::system_category())
					  .message());
			}
			nchars++;
		}
	}
	os.close();
}
