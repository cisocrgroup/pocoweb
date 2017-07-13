#include "SessionDirectory.hpp"
#include <boost/filesystem/operations.hpp>
#include "Session.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::SessionDirectory(const std::string& sid)
    : dirs_(), sid_(sid) {}

////////////////////////////////////////////////////////////////////////////////
SessionDirectory::~SessionDirectory() noexcept { close(); }

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::close() const noexcept {
	for (const auto& dir : dirs_) {
		boost::system::error_code ec;
		boost::filesystem::remove_all(dir, ec);
		if (ec) {
			CROW_LOG_WARNING
			    << "(SessionDirectory) Could not remove directory "
			    << dir << ": " << ec.message();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void SessionDirectory::init(const Path& dir) const {
	boost::filesystem::create_directories(dir);
}
