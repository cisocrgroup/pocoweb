#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
#include <regex>
#include <pugixml.hpp>
#include "LlocsPageParser.hpp"
#include "BadRequest.hpp"
#include "Page.hpp"
#include "Line.hpp"
#include "Box.hpp"
#include "util.hpp"

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
LlocsPageParser::LlocsPageParser(Path path)
	: dir_(std::move(path))
	, id_(std::stoi(dir_.filename().string(), nullptr, 16))
	, done_(false)
{
	assert(fs::is_directory(dir_));
}

////////////////////////////////////////////////////////////////////////////////
PagePtr 
LlocsPageParser::parse() 
{
	done_ = true; // one page per directory
	return parse();
}

////////////////////////////////////////////////////////////////////////////////
PagePtr
LlocsPageParser::parse() const
{
	std::vector<std::pair<Path, Path>> llocs;
	fs::directory_iterator b(dir_), e;
	BOOST_FOREACH(const auto& file, std::make_pair(b, e)) {
	// for (const auto& file: dir_) {
		auto pair = get_path_pair(file);
		if (pair) {
			llocs.push_back(*pair);
		}
	}
	std::sort(begin(llocs), end(llocs));
	auto page = std::make_shared<Page>(id_);
	for (int i = 0; i < static_cast<int>(llocs.size()); ++i) {
		page->push_back(parse_line(i + 1, llocs[i]));
	}
	return page;
}

////////////////////////////////////////////////////////////////////////////////
Line 
LlocsPageParser::parse_line(int i, const PathPair& p)
{
	static const std::regex linere{R"((.*)\t(-?\d*\.\d+)(\t(\d*\.\d+))?)"};
	std::ifstream is(p.first.string());
	if (not is.good())
		throw std::system_error(errno, std::system_category(), p.first.string());
	Line line(i);
	line.img = p.second;
	std::string tmp;
	while (std::getline(is, tmp)) {
		std::smatch m;
		if (not std::regex_match(tmp, m, linere))
			throw BadRequest(
				"(LLocsPageParser) Invalid llocs file: " +
				p.first.string() + " (" + tmp + ")"
			);
		double cut = std::stod(m[2]);
		double conf = 0;
		if (m[4].length())
			conf = std::stod(m[4]);
		if (cut < 0) // fix cuts smaller than 0 (does happen)
			cut = 0;
		line.append(m[1], 0, static_cast<int>(cut), conf);
	}
	return line;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<LlocsPageParser::PathPair> 
LlocsPageParser::get_path_pair(const Path& file)
{
	if (fs::is_regular_file(file) and file.extension() == ".llocs") {
		auto img = file;
		for (const auto ext: {".png", ".jpg", ".jpeg", ".tif", ".tiff"}) {
			img.replace_extension(ext);
			if (fs::is_regular_file(img)) 
				return std::make_pair(file, img);
			img.replace_extension(".dew");
			img += ext;
			if (fs::is_regular_file(img))
				return std::make_pair(file, img);
			img.replace_extension();
			img.replace_extension(".bin");
			img += ext;
			if (fs::is_regular_file(img))
				return std::make_pair(file, img);
			img.replace_extension();
		}
	}
	return {};
}

