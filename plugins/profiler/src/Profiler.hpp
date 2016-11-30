#ifndef profiler_Profiler_hpp__
#define profiler_Profiler_hpp__

#include <atomic>
#include <memory>

namespace pcw {
	class Book;
	using ConstBookSptr = std::shared_ptr<const pcw::Book>;
	class Profile;
	template<class T> class Maybe;
}

namespace profiler {
	using ConstBookSptr = pcw::ConstBookSptr;

	class Profiler {
	public:
		Profiler(ConstBookSptr book): book_(std::move(book)) {}
		virtual ~Profiler() noexcept = default;
		pcw::Maybe<pcw::Profile> profile() noexcept;

	protected:
		virtual pcw::Profile do_profile() = 0;
		const pcw::Book& book() const noexcept {return *book_;}

	private:
		ConstBookSptr book_;
	};
}

#endif // profiler_Profiler_hpp__
