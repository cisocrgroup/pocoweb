#ifndef pcw_PackageBuilder_hpp__
#define pcw_PackageBuilder_hpp__

#include <memory>
#include <vector>

namespace pcw {
	class Project;
	using ProjectSptr = std::shared_ptr<Project>;
	class Package;
	using PackageSptr = std::shared_ptr<Package>;

	class PackageBuilder {
	public:
		void set_project(Project& project);
		std::vector<PackageSptr> build() const;
		void set_random(bool r=true)
		{
			random_ = r;
		}
		void set_continous(bool c=true)
		{
			continous_ = c;
		}
		void set_number(size_t n)
		{
			number_ = n;
		}

	private:
		std::vector<PackageSptr> build_random() const;
		std::vector<PackageSptr> build_continous() const;
		std::vector<PackageSptr> build_simple() const;

		ProjectSptr project_;
		size_t number_;
		bool continous_, random_;
	};
}

#endif // pcw_PackageBuilder_hpp__
