#ifndef pcw_Box_hpp__
#define pcw_Box_hpp__

#include <vector>
#include <tuple>

namespace pcw {
	struct Box {
		Box(int l = 0, int t = 0, int r = 0, int b = 0);
		int left() const noexcept {return left_;}
		int right() const noexcept {return right_;}
		int top() const noexcept {return top_;}
		int bottom() const noexcept {return bottom_;}
		void set_left(int x) noexcept {left_ = x;}
		void set_right(int x) noexcept {right_ = x;}
		void set_top(int x) noexcept {top_ = x;}
		void set_bottom(int x) noexcept {bottom_ = x;}
		int width() const noexcept {return right_ - left_;}
		int height() const noexcept {return bottom_ - top_;}

		Box& increase_left(int d) noexcept;
		Box& increase_right(int d) noexcept;
		Box& increase_top(int d) noexcept;
		Box& increase_bottom(int d) noexcept;
		Box& increase(int d) noexcept;

		Box& operator+=(const Box& other);

		std::vector<Box> split(int n) const;

	private:
		int left_, top_, right_, bottom_;
	};

	static inline bool
	operator==(const Box& a, const Box& b) noexcept {
		return std::make_tuple(a.left(), a.top(), a.right(), a.bottom()) ==
			std::make_tuple(b.left(), b.top(), b.right(), b.bottom());
	}
	static inline bool
	operator<(const Box& a, const Box& b) noexcept {
		return std::make_tuple(a.left(), a.top(), a.right(), a.bottom()) <
			std::make_tuple(b.left(), b.top(), b.right(), b.bottom());
	}

	template<class C>
	inline std::basic_ostream<C>&
	operator<<(std::basic_ostream<C>& os, const Box& box) {
		return os << "bbox "
			  << box.left() << " " << box.top() << " "
			  << box.right() << " " << box.bottom();
	}
}

#endif // pcw_Box_hpp__
