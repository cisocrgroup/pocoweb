#include "Archiver.hpp"
#include "Project.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
Archiver::Archiver(const Project& project)
    : project_(project.shared_from_this()) {}
