#include <boost/filesystem/operations.hpp>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "core/Line.hpp"
#include "OcropusLlocsParserPage.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserPage::write(const Path& path) const
{
	auto base = path / dir_;
	boost::filesystem::create_directory(base);
	int i = 0;
	for (const auto& line: lines()) {
		std::stringstream os;
		os << std::hex << std::setfill('0') << std::setw(10) << ++i;
		write(i, *line, base / os.str());
	}
}

////////////////////////////////////////////////////////////////////////////////
void
OcropusLlocsParserPage::write(int id, const ParserLine& line, const Path& path) const
{
	auto l = line.line(id);
	std::wofstream os(path.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), path.string());
	for (auto i = 0U; i < l->size(); ++i) {
		os << l->wstring()[i] << '\t'
		   << l->cuts()[i] << '\t'
		   << l->confidences()[i] << '\n';
	}
	os.close();
}
