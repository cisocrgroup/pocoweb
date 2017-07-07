#include "llocs.hpp"

////////////////////////////////////////////////////////////////////////////////
pcw::Path pcw::get_llocs_from_png(const Path& path) {
	static const std::string ext = ".llocs";
	auto const file = path.stem().replace_extension(ext);
	const auto pdir = path.parent_path();
	return pdir / file;
}
