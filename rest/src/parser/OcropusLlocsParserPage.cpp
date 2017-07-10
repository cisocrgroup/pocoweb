#include "OcropusLlocsParserPage.hpp"
#include <crow/logging.h>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "core/Line.hpp"
#include "llocs.hpp"

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
	std::wofstream os(llocs.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(),
					llocs.string());
	for (const auto& c : l->chars()) {
		auto cc = c.get_cor();
		if (cc) {
			os << cc << '\t' << static_cast<double>(c.cut) << '\t'
			   << c.conf << '\n';
		}
	}
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
void OcropusLlocsParserPage::copy(const Path& from, const Path& to) {
	boost::system::error_code ec;
	boost::filesystem::create_hard_link(from, to, ec);
	if (ec) {  // could not create hard link; try copy
		CROW_LOG_WARNING << "Could not create hardlink from " << from
				 << " to " << to << ": " << ec.message();
		boost::filesystem::copy_file(from, to, ec);
		if (ec) {
			CROW_LOG_WARNING << "Could not copy file from " << from
					 << " to " << to << ": "
					 << ec.message();
		}
	}
}
