#include <crow.h>
#include "core/util.hpp"
#include "core/App.hpp"
#include "core/Config.hpp"
#include "Login.hpp"
#include "core/Session.hpp"
#include "core/Password.hpp"
#include "database/NewDatabase.hpp"
#include "CreateUser.hpp"
#include "UpdateUser.hpp"
#include "DeleteUser.hpp"
#include "core/User.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
static void
insert_default_user(const std::string& p, const App& app)
{
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
			app.config().plugins[p].get<std::string>("default-institute")
		);
		if (not user)
			THROW(Error, "could not create default user");
		commiter.commit();
	}

}

////////////////////////////////////////////////////////////////////////////////
static const char*
do_plugin(const std::string& p, App& app) noexcept
{
	try {
		app.config().setup_logging();
		insert_default_user(p, app);

		auto login = std::make_unique<Login>();
		auto create_user = std::make_unique<CreateUser>();
		auto update_user = std::make_unique<UpdateUser>();
		auto delete_user = std::make_unique<DeleteUser>();

		app.Register(std::move(login));
		app.Register(std::move(create_user));
		app.Register(std::move(update_user));
		app.Register(std::move(delete_user));

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

