#include <string>
#include <memory>
#include <json.hpp>
#include "Box.hpp"
#include "Line.hpp"

///////////////////////////////////////////////////////////////////////////////
void
pcw::Line::load(nlohmann::json& json)
{
	id = json["lineid"];
	line_ = json["line"];
	cuts_.clear();
	for (const auto& cut: json["cuts"])
		cuts_.push_back(cut);
	box.load(json["box"]);
}

///////////////////////////////////////////////////////////////////////////////
void 
pcw::Line::store(nlohmann::json& json) const
{
	json["lineid"] = id;
	json["line"] = line_;
	json["cuts"] = cuts_;
	box.store(json["box"]);
}

///////////////////////////////////////////////////////////////////////////////
std::string 
pcw::Line::cuts_str() const noexcept
{
	// using std::begin;
	// using std::end;
	std::stringstream os;
	std::copy(begin(cuts_), end(cuts_), std::ostream_iterator<int>(os, " "));
	return os.str();
}
