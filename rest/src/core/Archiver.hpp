#ifndef pcw_Archiver_hpp__
#define pcw_Archiver_hpp__

#include "database/mysql.hpp"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
class Project;
class Line;
struct Config;
struct DbPackage;
struct DbLine;

class Archiver {
public:
  using Path = boost::filesystem::path;
  Archiver(int pid, MysqlConnection &conn, const Config &config);
  Path operator()() const;

private:
  void zip(const Path &dir, const Path &archive) const;
  void copy_files(const Path &dir, const DbPackage &package) const;
  void write_adaptive_token_set(const Path &dir) const;
  Path get_tmp_file(const Path &source, const Path &tmpdir) const;
  Path copy_to_tmp_dir(const Path &source, const Path &tmpdir) const;
  static Path archive_name(const DbPackage &package) noexcept;
  static void write_line_text_snippets(const DbLine &line, const Path &cor,
                                       const Path &ocr);
  static void write_text_snippet(const std::string &line, const Path &to);
  static Path remove_common_base_path(const Path &p, const Path &base);

  MysqlConnection &conn_;
  const Path basedir_;
  const int pid_;
};
} // namespace pcw
#endif // pcw_Archiver_hpp__
