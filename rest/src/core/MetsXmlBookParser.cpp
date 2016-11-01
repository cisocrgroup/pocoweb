#include "PageParser.hpp"
#include "Page.hpp"
#include "BadRequest.hpp"
#include "Book.hpp"
#include "MetsXmlBookParser.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
MetsXmlBookParser::MetsXmlBookParser(const Path& path)
	: XmlFile(path)
	, book_()
	, base_(path.parent_path())
{
}

////////////////////////////////////////////////////////////////////////////////
BookPtr
MetsXmlBookParser::parse() 
{
	xml_.traverse(*this);
	make_book();
	assert(book_);
	return book_;
}

////////////////////////////////////////////////////////////////////////////////
bool 
MetsXmlBookParser::begin(XmlNode& node) 
{
	book_ = std::make_shared<Book>();
	ids_.clear();
	map_.clear();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool 
MetsXmlBookParser::for_each(XmlNode& node) 
{
// <ns3:fileGrp ID="IMG">
//     <ns3:file ID="IMG_1" SEQ="1" MIMETYPE="image/jpeg" CREATED="2016-10-28T14:13:54.000+02:00">
//         <ns3:FLocat LOCTYPE="OTHER" OTHERLOCTYPE="FILE" ns2:type="simple" ns2:href="005401_0001_164689.jpg"/>
//     </ns3:file>
//     <ns3:file ID="IMG_2" SEQ="2" MIMETYPE="image/jpeg" CREATED="2016-10-28T14:13:57.000+02:00">

// <ns3:structMap ID="TRP_STRUCTMAP" TYPE="MANUSCRIPT">
//     <ns3:div ID="TRP_DOC_DIV" ADMID="MD_ORIG">
//         <ns3:div ID="PAGE_1" ORDER="1" TYPE="SINGLE_PAGE">
//             <ns3:fptr>
//                 <ns3:area FILEID="IMG_1"/>
//             </ns3:fptr>
//             <ns3:fptr>
//                 <ns3:area FILEID="PAGEXML_1"/>
//             </ns3:fptr>
//             <ns3:fptr>
//                 <ns3:area FILEID="ALTO_1"/>
//             </ns3:fptr>
//         </ns3:div>

	assert(book_);
	if (equal(node.parent().name(), "trpDocMetadata")) {
		if (equal(node.name(), "author"))
			book_->author = node.child_value();
		else if (equal(node.name(), "title"))
			book_->title = node.child_value();
		else if (equal(node.name(), "desc"))
			book_->description = node.child_value();
	} else if (is_flocat(node)) {
		ids_[node.parent().attribute("ID").value()] = {
			attr_value_ns(node.parent(), "MIMETYPE"),
			base_ / attr_value_ns(node, "href")
		};
	} else if (is_area(node)) {
		auto order = node.parent().parent().attribute("ORDER").as_int();
		map_[order].push_back(node.attribute("FILEID").value());
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
void
MetsXmlBookParser::make_book()
{
	using std::begin;
	using std::end;
	assert(book_);
	auto iif = [this](const auto& id) {return ids_[id].type == FileType::Img;};
	auto iof = [this](const auto& id) {
		switch (ids_[id].type) {
		case FileType::Other:
		case FileType::Img:
			return false;
		default:
			return true;
		}
	};

	std::vector<PagePtr> pages;
	for (const auto& grp: map_) {
		assert(not grp.second.empty());
		const auto b = begin(grp.second);
		const auto e = end(grp.second);
		const auto i = std::find_if(b, e, iif);
		const auto j = std::find_if(b, e, iof);
		if (i == e)
			throw BadRequest(
				"(MetsXmlBookParser) No image file for order: " + 
				std::to_string(grp.first)
			);
		if (j == e)
			throw BadRequest(
				"(MetsXmlBookParser) No ocr file for order: " + 
				std::to_string(grp.first)
			);
		
		auto pp = make_page_parser(ids_[*j].type, ids_[*j].href);
		assert(pp);
		while (pp->has_next()) {
			auto page = pp->parse();
			if (not page)
				continue;
			page->id = grp.first;
			page->img = ids_[*i].href;
			pages.push_back(page);
		}
	}
	for (const auto& page: pages) {
		assert(page);
		book_->push_back(page);
	}
	assert(book_);
}

////////////////////////////////////////////////////////////////////////////////
bool 
MetsXmlBookParser::equal(const char* uri, const char* str) noexcept
{
	return strcmp(uri, str) == 0;
}

////////////////////////////////////////////////////////////////////////////////
bool 
MetsXmlBookParser::equal_ns(const char* uri, const char* str) noexcept
{
	auto local = strrchr(uri, ':');
	return local ? equal(local + 1, str) : equal(uri, str);
}

////////////////////////////////////////////////////////////////////////////////
bool
MetsXmlBookParser::is_flocat(const XmlNode& node) noexcept
{
	// <ns3:fileGrp ID="IMG">
	//     <ns3:file ID="IMG_1" SEQ="1" MIMETYPE="image/jpeg" CREATED="2016-10-28T14:13:54.000+02:00">
	//         <ns3:FLocat LOCTYPE="OTHER" OTHERLOCTYPE="FILE" ns2:type="simple" ns2:href="005401_0001_164689.jpg"/>
	return equal_ns(node.name(), "FLocat") and 
		equal_ns(node.parent().name(), "file") and 
		equal_ns(node.parent().parent().name(), "fileGrp");
}

////////////////////////////////////////////////////////////////////////////////
bool
MetsXmlBookParser::is_area(const XmlNode& node) noexcept
{
	// <ns3:div ID="PAGE_1" ORDER="1" TYPE="SINGLE_PAGE">
	//     <ns3:fptr>
	//         <ns3:area FILEID="IMG_1"/>
	//     </ns3:fptr>
	return equal_ns(node.name(), "area") and 
		equal_ns(node.parent().name(), "fptr") and 
		equal_ns(node.parent().parent().name(), "div");
}

////////////////////////////////////////////////////////////////////////////////
const char* 
MetsXmlBookParser::attr_value_ns(const XmlNode& node, const char* name) noexcept
{
	static const char* EMPTY = "";
	for (const auto& attr: node.attributes()) {
		if (equal_ns(attr.name(), name))
			return attr.value();
	}
	return EMPTY;	
}

////////////////////////////////////////////////////////////////////////////////
MetsXmlBookParser::FileGroup::FileGroup(const char *mimetype, Path path)
	: href(std::move(path))
{
	if (strstr(mimetype, "image/") == mimetype) // starts with 'image/...'
		type = FileType::Img;
	else if (strcmp(mimetype, "application/xml") == 0)
		type = get_file_type(href);
	else
		type = FileType::Other;
}
