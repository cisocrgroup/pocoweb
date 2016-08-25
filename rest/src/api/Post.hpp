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
#include "BookDir.hpp"
#include "Config.hpp"
#include "Api.hpp"

//
// PostBook
//
namespace pcw {
	template<class S> class PostBook: public Api<S, PostBook<S>> {
	public:
		using Base = typename pcw::Api<S, PostBook<S>>::Base;
		using Server = typename Base::Server;
		using Status = typename Base::Status;
		using Content = typename Base::Content;

		void do_reg(Server& server) const noexcept;
		Status run(Content& content) const;

	private:
		std::string generate_book_dir() const;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
void 
pcw::PostBook<S>::do_reg(Server& server) const noexcept 
{
	server.server.resource["^/new-book/title/([^/]+)/author/([^/]+)$"]["POST"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::PostBook<S>::Status
pcw::PostBook<S>::run(Content& content) const 
{
	BOOST_LOG_TRIVIAL(info) << "(PostBook) title: " 
				<< content.req->path_match[1] 
				<< " author: " << content.req->path_match[2];
	if (not content.session)
		return Status::Forbidden;
	
	// get new book	
	Books books(content.session);
	auto book = books.new_book(
		content.req->path_match[1], 
		content.req->path_match[2],
		generate_book_dir()
	);

	// write book
	using json = nlohmann::json;
	json j, jj;
	j["api"] = PCW_API_VERSION;
	book->store(jj);
	j["book"] = jj;

	// update content
	content.os << j << "\n";
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

//
// PostPageImage
//
namespace pcw {
	template<class S> class PostPageImage: public Api<S, PostPageImage<S>> {
	public:
		using Base = typename pcw::Api<S, PostPageImage<S>>::Base;
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
pcw::PostPageImage<S>::do_reg(Server& server) const noexcept 
{
	server.server.resource["^/books/(\\d+)/pages/(\\d+)/([^/]+)$"]["POST"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::PostPageImage<S>::Status
pcw::PostPageImage<S>::run(Content& content) const noexcept
{
	BOOST_LOG_TRIVIAL(info) << "(PostPageImage) book: " 
				<< content.req->path_match[1] 
				<< " page: " << content.req->path_match[2]
				<< " ext: " << content.req->path_match[3];
	if (not content.session)
		return Status::Forbidden;

	const auto bookid = std::stoi(content.req->path_match[1]);
	const auto pageid = std::stoi(content.req->path_match[2]);
	const auto ext = content.req->path_match[3];
	auto book = content.session->current_book;
	Books books(content.session);
	
	if (not book or book->data.id != bookid) 
		book = books.find_book(bookid);
	if (not book or not books.is_allowed(*book))
		return Status::Forbidden;
	
	// we have a valid book
	BookDir book_dir(*book);
	auto page = book_dir.add_page_image(pageid, ext, content.req->content);

	// invalid page id
	if (not page or page->id < 1)
		return Status::BadRequest;
	// add page to book
	if (book->size() <= static_cast<size_t>(page->id))
		book->resize(page->id);
	(*book)[page->id] = page;	
	content.session->current_book = book;
	return Status::Created;
}

//
// PostPageXml
//
namespace pcw {
	template<class S> class PostPageXml: public Api<S, PostPageXml<S>> {
	public:
		using Base = typename pcw::Api<S, PostPageXml<S>>::Base;
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
pcw::PostPageXml<S>::do_reg(Server& server) const noexcept 
{
	server.server.resource["^/books/(\\d+)/pages/(\\d+)/xml$"]["POST"] = *this;
}

////////////////////////////////////////////////////////////////////////////////
template<class S>
typename pcw::PostPageXml<S>::Status
pcw::PostPageXml<S>::run(Content& content) const noexcept
{
	BOOST_LOG_TRIVIAL(info) << "(PostPageXml) book: " 
				<< content.req->path_match[1] 
				<< " page: " << content.req->path_match[2];
	if (not content.session)
		return Status::Forbidden;

	const auto bookid = std::stoi(content.req->path_match[1]);
	const auto pageid = std::stoi(content.req->path_match[2]);
	auto book = content.session->current_book;
	Books books(content.session);
	
	// do we have a valid book?
	if (not book or book->data.id != bookid) 
		book = books.find_book(bookid);
	if (not book or not books.is_allowed(*book))
		return Status::Forbidden;
	
	// check pageid
	if (pageid < 1 or 
	    book->size() <= static_cast<size_t>(pageid) or 
	    not (*book)[pageid])
		return Status::BadRequest;
	auto page = (*book)[pageid];
	assert(page and page->id == pageid);

	// parse ocr (xml)
	BookDir book_dir(*book);
	book_dir.add_page_ocr(*page, content.req->content);

	// update database
	books.insert_page(*book, *page);
	content.session->current_book = book;
	return Status::Created;
}

#endif // api_Post_hpp__
