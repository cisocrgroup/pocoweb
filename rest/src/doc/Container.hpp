#ifndef pcw_Container_hpp__
#define pcw_Container_hpp__

namespace pcw { 
	template<class T>
	class Container {
	public:
		using Ptr = std::shared_ptr<T>;
		using Ts = std::vector<Ptr>;
		using const_iterator = typename Ts::const_iterator;
		using iterator = typename Ts::iterator;
		using value_type = typename Ts::value_type;

		virtual ~Container() noexcept = default;
		size_t size() const noexcept {return ts_.size();}
		bool empty() const noexcept {return ts_.empty();}
		const Ptr& operator[](size_t i) const {return ts_.at(i);}
		Ptr& operator[](size_t i) {return ts_[i];}
		const Ptr& back() const noexcept {return ts_.back();}
		Ptr& back() noexcept {return ts_.back();}
		void push_back(Ptr p) {ts_.push_back(std::move(p));}
		void resize(size_t n) {ts_.resize(n);}
		void clear() {ts_.clear();}

		auto begin() const noexcept {return ts_.begin();}
		auto end() const noexcept {return ts_.end();}
		auto begin() noexcept {return ts_.begin();}
		auto end() noexcept {return ts_.end();}

	private:
		Ts ts_;
	};
}

#endif // pcw_Container_hpp__
