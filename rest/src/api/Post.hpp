#ifndef api_Post_hpp__
#define api_Post_hpp__

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "util/hash.hpp"
#include "db/db.hpp"
#include "db/User.hpp"
#include "db/Books.hpp"
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
		std::string generate_book_dir() const;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
void 
pcw::PostBook<S>::do_reg(Server& server) const noexcept 
{
	server.server.resource["^/new-book/title/([^/]+)$"]["POST"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::PostBook<S>::Status
pcw::PostBook<S>::run(Content& content) const noexcept
{
	if (not content.session)
		return Status::Forbidden;
	
	// get new book	
	Books books(content.session);
	auto book = books.new_book(content.req->path_match[1], generate_book_dir());

	// write book
	using json = nlohmann::json;
	json j, jj;
	j["api"] = PCW_API_VERSION;
	book->store(jj);
	j["book"] = jj;

	// update content
	content.os << j;
	content.session->current_book = book;
	return Status::Ok;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
std::string 
pcw::PostBook<S>::generate_book_dir() const
{
	// TODO: What happens if two threads generate the same dir? FIX THIS!
	boost::filesystem::path bdir{this->config().daemon.basedir};
	while (true) {
		const auto book_dir = "book-" + gensessionid(16);
		auto dir = bdir / book_dir;
		if (not boost::filesystem::exists(dir))
			return dir.string();
	}
	throw std::logic_error("(PostBook::generate_book_dir) "
			       "could not generate unique book directory");
}

#endif // api_Post_hpp__
