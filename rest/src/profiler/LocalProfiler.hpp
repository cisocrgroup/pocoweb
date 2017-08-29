#ifndef profiler_LocalProfiler_hpp__
#define profiler_LocalProfiler_hpp__

#include <boost/filesystem/path.hpp>
#include "Profiler.hpp"
#include "utils/TmpDir.hpp"

namespace pcw {
using Path = boost::filesystem::path;
class Config;

class LocalProfiler : public Profiler {
       public:
	LocalProfiler(ConstBookSptr book, const Config& config);
	virtual ~LocalProfiler() noexcept override = default;

       protected:
	virtual pcw::Profile do_profile() override;
	virtual std::vector<std::string> do_languages() override;

       private:
	void run_profiler(const std::string& command) const;
	pcw::Profile read_profile() const;
	void write_docxml() const;
	std::string profiler_command() const;
	std::string profiler_config() const;

	const TmpDir tmpdir_;
	const Path backend_, outfile_, infile_;
	const bool debug_;
};
}

#endif  // profiler_LocalProfiler_hpp__
