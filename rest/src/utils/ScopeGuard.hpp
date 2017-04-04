#ifndef pcw_ScopeGuard_hpp__
#define pcw_ScopeGuard_hpp__

namespace pcw {
	class ScopeGuard {
	public:
		template<class Callable>
		ScopeGuard(Callable&& undo_func)
			: f_(std::forward<Callable>(undo_func)) {}

		ScopeGuard(ScopeGuard&& other)
			: f_(std::move(other.f_))
		{
			other.f_ = nullptr;
		}

		~ScopeGuard() noexcept
		{
			if(f_) f_(); // must not throw
		}

		void dismiss() noexcept
		{
			f_ = nullptr;
		}

		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;

	private:
		std::function<void()> f_;
	};
}
#endif // pcw_ScopeGuard_hpp__
