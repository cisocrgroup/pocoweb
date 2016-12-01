#include <cstdlib>
#include <thread>
#include <fstream>
#include <crow/logging.h>
#include "core/docxml.hpp"
#include "core/Book.hpp"
#include "core/Error.hpp"
#include "core/Profile.hpp"
#include "Config.hpp"
#include "LocalProfiler.hpp"

using namespace profiler;

////////////////////////////////////////////////////////////////////////////////
pcw::Profile
LocalProfiler::do_profile()
{
	CROW_LOG_INFO << "(LocalProfiler) Profiling book id: " << book().id();
	write_docxml();
	run_profiler(profiler_command());
	CROW_LOG_INFO << "(LocalProfiler) Done profiling book id: " << book().id();
	return read_profile();
}

////////////////////////////////////////////////////////////////////////////////
void
LocalProfiler::run_profiler(const std::string& command) const
{
	CROW_LOG_DEBUG << "(LocalProfiler) Executing profiler command: " << command;
	auto res = system(command.data());
	CROW_LOG_DEBUG << "(LocalProfiler) Return code: " << res;
	switch (res) {
	case -1: throw std::system_error(errno, std::system_category(), "fork");
	case 127: THROW(pcw::Error, "Shell could not execute profiler: ", command);
	case 0: break;
	default: THROW(pcw::Error, "Profiler returned error code: ", res);
	}
}

////////////////////////////////////////////////////////////////////////////////
pcw::Profile
LocalProfiler::read_profile() const
{
	CROW_LOG_DEBUG << "(LocalProfiler) Reading profile file " << infile_;
	pcw::ProfileBuilder builder(
		std::dynamic_pointer_cast<const pcw::Book>(book().shared_from_this()));
	builder.add_candidates_from_file(infile_);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
void
LocalProfiler::write_docxml() const
{
	CROW_LOG_DEBUG << "(LocalProfiler) Writing doc xml file " << outfile_;
	pugi::xml_document xml;
	pcw::DocXml docxml(xml);
	docxml << book();
	std::ofstream os(outfile_.string());
	if (not os.good())
		throw std::system_error(errno, std::system_category(), outfile_.string());
	xml.save(os);
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
std::string
LocalProfiler::profiler_command() const
{
	return Config::get().exe().string() +
		" --adaptive" +
		" --config " + profiler_config() +
		" --outputFormat DOC_XML" +
		" --file " + outfile_.string() +
		" --doc_out " + infile_.string();
}

////////////////////////////////////////////////////////////////////////////////
std::string
LocalProfiler::profiler_config() const
{
	return (Config::get().backend() / book().lang + ".ini").string();
}
