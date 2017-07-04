#ifndef pcw_ProjectBuilder_hpp__
#define pcw_ProjectBuilder_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pcw {
	class Project;
	using ProjectSptr = std::shared_ptr<Project>;
	class Book;
	using ConstBookSptr = std::shared_ptr<const Book>;
	class Package;
	using PackageSptr = std::shared_ptr<Package>;

	class ProjectBuilder {
	public:
		ProjectBuilder(): project_(), book_() {reset();}

		ProjectBuilder& reset();
		ProjectBuilder& set_origin(const Book& book);
		const ProjectBuilder& set_project_id(int id) const;
		const ProjectBuilder& add_page(int pageid) const;
		ProjectSptr build() const;

	private:
		PackageSptr project_;
		ConstBookSptr book_;

	};
}
#endif // pcw_ProjectBuilder_hpp__
