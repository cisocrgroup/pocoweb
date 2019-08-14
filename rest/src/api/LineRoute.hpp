#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace crow {
namespace json {
class wvalue;
} // namespace json
} // namespace crow
namespace pcw {
struct Token;
class Line;
using Json = crow::json::wvalue;

class LineRoute : public CrtpRoute<LineRoute>
{
public:
  virtual ~LineRoute() noexcept override = default;
  virtual void Register(App& app) override;
  virtual const char* route() const noexcept override { return route_; }
  virtual const char* name() const noexcept override { return name_; }

  pcw_crtp_route_def_impl__(int, int, int);
  pcw_crtp_route_def_impl__(int, int, int, int);
  Response impl(HttpGet, const Request& req, int bid, int pid, int lid) const;
  Response impl(HttpPost, const Request& req, int bid, int pid, int lid) const;
  Response impl(HttpDelete, const Request& req, int pid, int p, int lid) const;
  Response impl(HttpGet, const Request& req, int pid, int p, int lid, int tid)
    const;
  Response impl(HttpPost, const Request& req, int pid, int p, int lid, int tid)
    const;

private:
  Response correct(MysqlConnection& conn,
                   int pid,
                   int p,
                   Line& line,
                   const std::string& c) const;
  Response correctx(MysqlConnection& conn,
                    int pid,
                    int tid,
                    Line& line,
                    const std::string& c) const;
  static boost::optional<Token> find_token(const Line& line, int tid);
  static void update_line(MysqlConnection& conn, const Line& line);
  static const char* route_;
  static const char* name_;
};

// class LineLoader {
// public:
//   LineLoader(MysqlConnection &c, int pid, int pageid, int lid)
//       : c_(c), pid_(pid), pageid_(pageid), lid_(lid) {}
//   std::shared_ptr<Line> loadLine();

// private:
//   MysqlConnection &c_;
//   int pid_, pageid_, lid_;
// };

class Corrector
{
public:
  Corrector(MysqlConnection& c, int pid, int pageid, int lid)
    : c_(c)
    , pid_(pid)
    , pageid_(pageid)
    , lid_(lid)
    , origin_()
  {}
  std::shared_ptr<Line> correctWholeLine(const std::string& correction);
  Json toJSON(const std::shared_ptr<Line>& line) const;

private:
  int loadOrigin() const;
  std::shared_ptr<Line> loadLine() const;
  void correct(Line& line, const std::string& correction) const;

  MysqlConnection& c_;
  int pid_, pageid_, lid_, origin_;
};
} // namespace pcw

#endif // pcw_LineRoute_hpp__
