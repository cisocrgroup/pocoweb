#include "OcropusLlocsPageParser.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include "OcropusLlocsParserLine.hpp"
#include "OcropusLlocsParserPage.hpp"
#include "ParserPage.hpp"
#include "core/Box.hpp"
#include "core/Line.hpp"
#include "core/Page.hpp"
#include "core/util.hpp"
#include "pugixml.hpp"
#include "utils/Error.hpp"

namespace fs = boost::filesystem;
using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
OcropusLlocsPageParser::OcropusLlocsPageParser(Path path)
    : dir_(std::move(path)),
      id_(std::stoi(dir_.filename().string(), nullptr, 10)),
      done_(false) {
	assert(fs::is_directory(dir_));
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr OcropusLlocsPageParser::parse() {
	assert(not done_);
	done_ = true;  // one page per directory
	return parse_page();
}

////////////////////////////////////////////////////////////////////////////////
ParserPagePtr OcropusLlocsPageParser::parse_page() const {
	std::vector<std::pair<Path, Path>> llocs;
	fs::directory_iterator b(dir_), e;
	BOOST_FOREACH (const auto& file, std::make_pair(b, e)) {
		auto pair = get_path_pair(file);
		if (pair) {
			llocs.push_back(*pair);
		}
	}
	std::sort(begin(llocs), end(llocs));
	auto page = std::make_shared<OcropusLlocsParserPage>(id_);
	for (int i = 0; i < static_cast<int>(llocs.size()); ++i) {
		page->lines().push_back(
		    std::make_shared<OcropusLlocsParserLine>(
			i + 1, llocs[i].first, llocs[i].second));
	}
	page->file_type = FileType::Llocs;
	return page;
}

////////////////////////////////////////////////////////////////////////////////
boost::optional<OcropusLlocsPageParser::PathPair>
OcropusLlocsPageParser::get_path_pair(const Path& file) {
	if (fs::is_regular_file(file) and file.extension() == ".llocs") {
		auto img = file;
		for (const auto ext :
		     {".png", ".jpg", ".jpeg", ".tif", ".tiff"}) {
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
