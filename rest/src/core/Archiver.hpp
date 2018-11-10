#ifndef pcw_Archiver_hpp__
#define pcw_Archiver_hpp__

#include "database/mysql.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Project;
class Line;
struct Config;

class Archiver
{
public:
  using Path = boost::filesystem::path;
  Archiver(const Project& p, MysqlConnection& conn, const Config& config);
  Path operator()() const;

private:
  void zip(const Path& dir, const Path& archive) const;
  void copy_files(const Path& dir) const;
  void write_adaptive_token_set(const Path& dir) const;
  void write_gt_file(const Line& line, const Path& to) const;
  Path archive_name() const noexcept;
  Path get_tmp_file(const Path& source, const Path& tmpdir) const;
  Path copy_to_tmp_dir(const Path& source, const Path& tmpdir) const;
  static Path remove_common_base_path(const Path& p, const Path& base);

  const std::shared_ptr<const Project> project_;
  MysqlConnection& conn_;
  const Path basedir_;
};
}
#endif // pcw_Archiver_hpp__
