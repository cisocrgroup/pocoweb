#include "Login.hpp"
#include "Logout.hpp"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "core/Password.hpp"
#include "core/Session.hpp"
#include "core/User.hpp"
#include "core/util.hpp"
#include "database/Database.hpp"
#include <crow.h>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static void
insert_default_user(const std::string& p, const App& app)
{
  try {
    auto conn = app.connection_pool().get_connection();
    assert(conn);
    auto name = app.config().plugins[p].get<std::string>("default-user");
    auto user = select_user(conn.db(), name);
    if (not user) {
      MysqlCommiter commiter(conn);
      user = create_user(
        conn.db(),
        name,
        app.config().plugins[p].get<std::string>("default-pass"),
        app.config().plugins[p].get<std::string>("default-email"),
        app.config().plugins[p].get<std::string>("default-institute"),
        true);
      if (not user)
        THROW(Error, "could not create default user");
      commiter.commit();
    }
  } catch (const std::exception& e) {
    CROW_LOG_ERROR << "could not insert default user: " << e.what();
  }
}

////////////////////////////////////////////////////////////////////////////////
static const char*
do_plugin(const std::string& p, App& app) noexcept
{
  try {
    app.config().setup_logging();
    insert_default_user(p, app);
    app.Register(std::make_unique<Login>());
    app.Register(std::make_unique<Logout>());
    return nullptr;
  } catch (const std::exception& e) {
    return what(e);
  }
}

extern "C" {
////////////////////////////////////////////////////////////////////////////////
const char*
plugin(const std::string& p, App& app)
{
  return do_plugin(p, app);
}
}
