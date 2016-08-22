#ifndef api_Post_hpp__
#define api_Post_hpp__

#include "util/hash.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/DbTableBooks.hpp"
#include "doc/BookData.hpp"
#include "doc/Book.hpp"
#include "doc/Line.hpp"
#include "doc/Page.hpp"
#include "Config.hpp"
#include "Api.hpp"

namespace pcw {
	template<class S> class PostBook: public Api<S, PostBook<S>> {
	public:
		using Base = typename pcw::Api<S, PostBook<S>>::Base;
		using Server = typename Base::Server;
		using Status = typename Base::Status;
		using Content = typename Base::Content;

		void do_reg(Server& server) const noexcept;
		Status run(Content& content) const noexcept;

	private:
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
void 
pcw::PostBook<S>::do_reg(Server& server) const noexcept 
{
	server.server.resource["^/book$"]["POST"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::PostBook<S>::Status
pcw::PostBook<S>::run(Content& content) const noexcept
{
	if (not content.session)
		return Status::Forbidden;
	Book book;
	return Status::InternalServerError;
}

#endif // api_Post_hpp__
