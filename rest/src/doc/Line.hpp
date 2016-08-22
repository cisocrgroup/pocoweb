#ifndef pcw_Line_hpp__
#define pcw_Line_hpp__

#include "Box.hpp"

namespace pcw {
	class Line: public std::enable_shared_from_this<Line> {
	public:
		using Cuts = std::vector<int>;

		Line() = default;
		Line(nlohmann::json& json): Line() {load(json);}
		virtual ~Line() noexcept = default;
		const std::string& line() const noexcept {return line_;}
		std::string& line() noexcept {return line_;}
		const Cuts& cuts() const noexcept {return cuts_;}
		Cuts& cuts() noexcept {return cuts_;}
		void load(nlohmann::json& json);
		void store(nlohmann::json& json) const;

		Box box;
		int id;

	private:
		std::string line_;
		Cuts cuts_;
	};
	using LinePtr = std::shared_ptr<Line>;
}

#endif // pcw_Line_hpp__

