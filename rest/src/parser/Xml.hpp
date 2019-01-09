#ifndef pcw_Xml_hpp__
#define pcw_Xml_hpp__

#include <boost/filesystem/path.hpp>
#include <memory>

namespace pugi {
class xml_node;
class xml_document;
}

namespace pcw {
using Path = boost::filesystem::path;
class Xml
{
public:
  using Doc = pugi::xml_document;
  using Node = pugi::xml_node;
  using DocPtr = std::shared_ptr<pugi::xml_document>;

  explicit Xml(DocPtr doc = nullptr);
  explicit Xml(const Path& path);
  virtual ~Xml() noexcept = default;
  Doc& doc() noexcept { return *doc_; }
  const Doc& doc() const noexcept { return *doc_; }
  DocPtr& doc_ptr() noexcept { return doc_; }
  const DocPtr& doc_ptr() const noexcept { return doc_; }
  void read(const Path& path);
  void write(const Path& path) const;

private:
  DocPtr doc_;
};
}

#endif // pcw_Xml_hpp__
