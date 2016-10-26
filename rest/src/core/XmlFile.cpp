#include "XmlFile.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
XmlFile::XmlFile(): xml_(), path_() {}

////////////////////////////////////////////////////////////////////////////////
XmlFile::XmlFile(const Path& path): XmlFile()
{
	read(path);
}

////////////////////////////////////////////////////////////////////////////////
XmlFile::XmlFile(const std::string& content): XmlFile()
{
	read(content);
}

////////////////////////////////////////////////////////////////////////////////
void
XmlFile::read(const Path& path)
{
	path_ = path;
	auto ok = xml_.load_file(path_.string().data());
	if (not ok)
		throw std::runtime_error(
			"(XmlFile) Could not read xml file " +
			path_.string() + ": " + ok.description()
		);
}

////////////////////////////////////////////////////////////////////////////////
void
XmlFile::read(const std::string& content)
{
	path_.clear();
	auto ok = xml_.load_string(content.data());
	if (not ok)
		throw std::runtime_error(
			"(XmlFile) Could not read xml file: " +
			std::string(ok.description())
		);
}

