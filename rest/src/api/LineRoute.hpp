#ifndef pcw_LineRoute_hpp__
#define pcw_LineRoute_hpp__

#include "core/CrtpRoute.hpp"

namespace pcw {
struct Token;
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
                   Line& line,
                   const std::string& c) const;
  Response correct(MysqlConnection& conn,
                   Line& line,
                   int tid,
                   const std::string& c) const;
  static boost::optional<Token> find_token(const Line& line, int tid);
  static void update_line(MysqlConnection& conn, const Line& line);
  static const char* route_;
  static const char* name_;
};
}

#endif // pcw_LineRoute_hpp__
