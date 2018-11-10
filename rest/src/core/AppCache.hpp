#ifndef pcw_AppCache_hpp__
#define pcw_AppCache_hpp__

#include "Book.hpp"
#include "Cache.hpp"
#include "Project.hpp"
#include <memory>

namespace pcw {
using ProjectCache = Cache<Project>;
using BookCache = Cache<Book>;
struct AppCache;
using CachePtr = std::shared_ptr<AppCache>;
using CacheSptr = std::shared_ptr<AppCache>;
struct AppCache
{
  AppCache(int ps, int bs)
    : projects(ps)
    , books(bs)
  {}
  ProjectCache projects;
  BookCache books;
};
}

#endif // pcw_AppCache_hpp__
