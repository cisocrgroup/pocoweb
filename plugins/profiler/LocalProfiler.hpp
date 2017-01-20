#ifndef profiler_LocalProfiler_hpp__
#define profiler_LocalProfiler_hpp__

#include <boost/filesystem/path.hpp>
#include "utils/TmpDir.hpp"
#include "Profiler.hpp"

namespace profiler {
	using Path = boost::filesystem::path;

	class LocalProfiler: public Profiler {
	public:
		LocalProfiler(ConstBookSptr book)
			: Profiler(std::move(book))
			, tmpdir_()
			, outfile_(tmpdir_ / "out.xml")
			, infile_(tmpdir_ / "in.xml")
		{}
		virtual ~LocalProfiler() noexcept override = default;

	protected:
		virtual pcw::Profile do_profile() override;

	private:
		void run_profiler(const std::string& command) const;
		pcw::Profile read_profile() const;
		void write_docxml() const;
		std::string profiler_command() const;
		std::string profiler_config() const;

		// static std::string exec(const std::string& command);

		const pcw::TmpDir tmpdir_;
		const Path outfile_, infile_;
	};
}

#endif // profiler_LocalProfiler_hpp__
