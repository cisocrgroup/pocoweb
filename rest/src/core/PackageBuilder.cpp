#include "utils/Error.hpp"
#include "Book.hpp"
#include "User.hpp"
#include "PackageBuilder.hpp"
#include "Package.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
void
PackageBuilder::set_project(Project& project)
{
	project_ = project.shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<PackageSptr>
PackageBuilder::build() const
{
	if (number_ <= 0)
		THROW(Error, "number of packages must be bigger than 0");
	if (not project_)
		return {};
	if (number_ > project_->size())
		THROW(Error, "number of packages must not be bigger than number of pages");
	if (random_)
		return build_random();
	if (continous_)
		return build_continous();
	else
		return build_simple();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<PackageSptr>
PackageBuilder::build_random() const
{
	assert(project_);
	assert(random_);
	assert(number_ > 0);
	assert(number_ <= project_->size());
	return {};
}

////////////////////////////////////////////////////////////////////////////////
std::vector<PackageSptr>
PackageBuilder::build_continous() const
{
	assert(project_);
	assert(continous_);
	assert(number_ > 0);
	assert(number_ <= project_->size());
	return {};
}

////////////////////////////////////////////////////////////////////////////////
std::vector<PackageSptr>
PackageBuilder::build_simple() const
{
	assert(project_);
	assert(not continous_);
	assert(number_ > 0);
	assert(number_ <= project_->size());

	std::vector<PackageSptr> res(number_);
	auto owner = project_->owner().shared_from_this();
	std::generate(begin(res), end(res), [&]() {
		return std::make_shared<Package>(0, *owner, project_->origin());
	});
	size_t i = 0;
	for (const auto& page: *project_) {
		res[i++ % number_]->push_back(*page);
	}
	return res;
}
