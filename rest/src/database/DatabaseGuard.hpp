#ifndef pcw_DatabaseGuard_hpp__
#define pcw_DatabaseGuard_hpp__

#include "utils/ScopeGuard.hpp"

namespace pcw {
	template<class Db> class Connection;

	template<class Db, class Derived>
	class DatabaseGuardBase {
	public:
		using connection_t = Connection<Db>;

		DatabaseGuardBase(connection_t& c)
			: c_(c)
			, sg_([this](){this->undo();})
		{}
		~DatabaseGuardBase() = default;
		DatabaseGuardBase(const DatabaseGuardBase&) = delete;
		DatabaseGuardBase& operator=(const DatabaseGuardBase&) = delete;

		void undo() noexcept
		{
			derived().undo_impl(c_);
		}

		void dismiss() noexcept
		{
			derived().dismiss_impl(c_);
			sg_.dismiss();
		}

	private:
		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}

		connection_t& c_;
		ScopeGuard sg_;
	};

	template<class Db>
	class DatabaseGuard: public DatabaseGuardBase<Db, DatabaseGuard<Db>> {
	public:
		using Base = DatabaseGuardBase<Db, DatabaseGuard<Db>>;
		DatabaseGuard(Connection<Db>& c): Base(c) {}
		void undo_impl(Connection<Db>&) noexcept {}
		void dismiss_impl(Connection<Db>&) noexcept {}
	};
}

#endif // pcw_DatabaseGuard_hpp__
