#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

namespace pcw {
	struct Box;
	class Line: public std::enable_shared_from_this<Line> {
	public:
		using Cuts = std::vector<int>;

		virtual ~Line() noexcept = default;
		const std::string& line() const noexcept {return line_;}
		std::string& line() noexcept {return line_;}
		const Cuts& cuts() const noexcept {return cuts_;}
		Cuts& cuts() noexcept {return cuts_;}
		Box box;
		int id;

	private:
		std::string line_;
		Cuts cuts_;
	};
	using LinePtr = std::shared_ptr<Line>;
}

#endif // pcw_Line_hpp__

