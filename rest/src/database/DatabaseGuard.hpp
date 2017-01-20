#ifndef pcw_DatabaseGuard_hpp__
#define pcw_DatabaseGuard_hpp__

#include "core/ScopeGuard.hpp"

namespace pcw {
	template<class Db> class Connection;

	template<class Db, class Derived>
	class DatabaseGuard {
	public:
		using connection_t = Connection<Db>;

		DatabaseGuard(connection_t& c)
			: c_(c)
			, sg_([this](){this->undo();})
		{}
		~DatabaseGuard() = default;
		DatabaseGuard(const DatabaseGuard&) = delete;
		DatabaseGuard& operator=(const DatabaseGuard&) = delete;

		void undo() noexcept
		{
			static_cast<Derived&>(*this).undo_impl(c_);
		}

		void dismiss() noexcept
		{
			sg_.dismiss();
		}

	private:
		connection_t& c_;
		ScopeGuard sg_;
	};
}

#endif // pcw_DatabaseGuard_hpp__
