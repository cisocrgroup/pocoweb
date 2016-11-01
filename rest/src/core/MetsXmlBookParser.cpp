#include "Book.hpp"
#include "MetsXmlBookParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
MetsXmlBookParser::MetsXmlBookParser(const Path& path)
	: XmlFile(path)
	, book_()
{
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
MetsXmlBookParser::parse() 
{
	book_ = std::make_shared<Book>();
	xml_.traverse(*this);
	return book_;
}

////////////////////////////////////////////////////////////////////////////////
bool 
MetsXmlBookParser::for_each(XmlNode& node) 
{
	assert(book_);
	if (strcmp("trpDocMetadata", node.parent().name()) == 0) {
		if (strcmp("author", node.name())) 
			book_->author = node.child_value();
		else if (strcmp("title", node.name()))
			book_->title = node.child_value();
		else if (strcmp("desc", node.name()))
			book_->description = node.child_value();
	} 
	std::cerr << "[debug] " << "METS node name: " << node.name() << "\n";
	return true;
}
