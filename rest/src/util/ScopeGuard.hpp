#ifndef pcw_ScopeGuard_hpp__
#define pcw_ScopeGuard_hpp__

namespace pcw {
	class ScopeGuard {
	public: 
		template<class Callable> 
		ScopeGuard(Callable && undo_func) 
			: f(std::forward<Callable>(undo_func)) {}

		ScopeGuard(ScopeGuard && other) 
			: f(std::move(other.f)) {
				other.f = nullptr;
			}

		~ScopeGuard() noexcept {
			if(f) f(); // must not throw
		}

		void dismiss() noexcept {
			f = nullptr;
		}

		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;

	private:
		std::function<void()> f;
	};
}
#endif // pcw_ScopeGuard_hpp__
