#include "RemoteProfiler.hpp"
#include <crow/logging.h>
#include <curl/curl.h>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <sstream>
#include "Profile.hpp"
#include "RemoteProfilerTemplate.hpp"
#include "docxml.hpp"
#include "pugixml.hpp"
#include "utils/Base64.hpp"
#include "utils/Error.hpp"

using namespace pcw;

/*
 * static int curl_trace(CURL* handle, curl_infotype type, char* data, size_t
 *size,
 *		      void* userp);
 */
static std::string get_namespace_prefix(pugi::xml_node node);

////////////////////////////////////////////////////////////////////////////////
struct curl_clean {
	void operator()(void* curl) const noexcept { curl_easy_cleanup(curl); }
};

using curl_ptr = std::unique_ptr<CURL, curl_clean>;

////////////////////////////////////////////////////////////////////////////////
struct curl_slist_clean {
	void operator()(curl_slist* slist) const noexcept {
		curl_slist_free_all(slist);
	}
};

using curl_slist_ptr = std::unique_ptr<curl_slist, curl_slist_clean>;

////////////////////////////////////////////////////////////////////////////////
static curl_ptr new_curl() {
	std::unique_ptr<CURL, curl_clean> curl(curl_easy_init());
	if (not curl) THROW(Error, "could not intialize curl");
	return curl;
}

////////////////////////////////////////////////////////////////////////////////
Profile RemoteProfiler::do_profile() {
	const auto path = boost::filesystem::path(url_) / "getProfile";
	char EBUF[CURL_ERROR_SIZE + 1];
	buffer_.clear();
	auto curl = new_curl();
	curl_easy_setopt(curl.get(), CURLOPT_URL, path.string().data());
	curl_slist_ptr slist = nullptr;
	slist.reset(
	    curl_slist_append(slist.release(), "Content-Type: text/xml"));
	curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, slist.get());
	curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, EBUF);
	curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &write);
	curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, this);
	// curl_easy_setopt(curl.get(), CURLOPT_DEBUGFUNCTION, curl_trace);
	// curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, 1L);
	std::stringstream xx, out;
	DocXml doc;
	doc << book();
	xx << doc;
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	in.push(boost::iostreams::gzip_compressor());
	in.push(xx);
	boost::iostreams::copy(in, out);
	RemoteProfilerTemplate tmpl;
	tmpl.set_userid("pocoweb")
	    .set_language(book().origin().data.lang)
	    .set_filename("tmp")
	    .set_data(out.str());
	// std::ofstream fileout("tmpl.xml");
	// fileout << tmpl.get();
	// fileout.close();
	curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE, tmpl.get().size());
	curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, tmpl.get().data());
	CROW_LOG_INFO << "(RemoteProfiler) sending profile";
	if (curl_easy_perform(curl.get()) != CURLE_OK)
		THROW(Error, "curl_easy_perfrom(): ", EBUF);
	return parse_profile(buffer_, book());
}

////////////////////////////////////////////////////////////////////////////////
Profile RemoteProfiler::parse_profile(const std::string& buffer,
				      const Book& book) {
	CROW_LOG_INFO << "(RemoteProfiler) parsing profile";
	pugi::xml_document doc;
	const auto ok = doc.load_string(buffer.data());
	if (not ok) {
		THROW(Error, "(RemoteProfiler) could not parse response: ",
		      ok.description());
	}
	const auto pre = get_namespace_prefix(doc.document_element());
	const auto ret = doc.document_element()
			     .child((pre + "returncode").data())
			     .child_value();
	if (ret != std::string("0")) {
		THROW(Error, "(RemoteProfiler) invalid return code: ", ret);
	}
	const auto msg = doc.document_element()
			     .child((pre + "message").data())
			     .child_value();
	if (msg != std::string("ok")) {
		THROW(Error, "(RemoteProfiler) invalid status message: ", msg);
	}
	const auto bin = doc.document_element()
			     .child((pre + "doc_out").data())
			     .child((pre + "binaryData").data())
			     .child_value();
	if (!bin || *bin == 0) {
		THROW(Error, "(RemoteProfiler) empty binary data");
	}
	std::stringstream gzipped(base64::decode(bin));
	std::stringstream unzipped;
	boost::iostreams::filtering_ostream out;
	out.push(boost::iostreams::gzip_decompressor());
	out.push(unzipped);
	boost::iostreams::copy(gzipped, out);
	ProfileBuilder builder(book.book_ptr());
	builder.add_candidates_from_stream(unzipped);
	return builder.build();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> RemoteProfiler::do_languages() {
	const auto path = boost::filesystem::path(url_) / "getConfigurations";
	char EBUF[CURL_ERROR_SIZE + 1];
	buffer_.clear();
	auto curl = new_curl();
	// curl_easy_setopt(curl.get(), CURLOPT_PORT, port);
	curl_easy_setopt(curl.get(), CURLOPT_URL, path.string().data());
	curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, EBUF);
	// curl_easy_setopt(curl.get(), CURLOPT_COOKIEFILE,
	// cookiefile.string().data());
	// curl_easy_setopt(curl.get(), CURLOPT_COOKIEJAR,
	// cookiefile.string().data());
	curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &write);
	curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, this);
	// curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION, &read);
	// curl_easy_setopt(curl.get(), CURLOPT_READDATA, this);
	if (curl_easy_perform(curl.get()) != CURLE_OK)
		THROW(Error, "curl_easy_perfrom(): ", EBUF);
	long http_code = 0;
	curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200) THROW(Error, "invalid return code: ", http_code);
	CROW_LOG_DEBUG << "(RemoteProfiler) languages: " << buffer_;
	return parse_languages(buffer_);
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> RemoteProfiler::parse_languages(
    const std::string& data) {
	pugi::xml_document xml;
	const auto res = xml.load_buffer(data.data(), data.size());
	if (not res) {
		THROW(Error, "could not get languages: ", res.description());
	}
	const auto pre = get_namespace_prefix(xml.document_element());
	auto nodes = xml.document_element().select_nodes(
	    (".//" + pre + "configurations").data());
	std::vector<std::string> languages;
	for (const auto& node : nodes) {
		languages.push_back(node.node().child_value());
	}
	return languages;
}

// ////////////////////////////////////////////////////////////////////////////////
// size_t RemoteProfiler::read(void *ptr, size_t size, size_t nmemb,
// void
// *userdata)
// {
// 	auto is = reinterpret_cast<std::ifstream*>(userdata);
// 	is->read(static_cast<char*>(ptr), size * nmemb);
// 	auto res = is->gcount();
// 	return res;
// }

////////////////////////////////////////////////////////////////////////////////
size_t RemoteProfiler::write(char* ptr, size_t size, size_t nmemb,
			     void* userdata) {
	auto that = reinterpret_cast<RemoteProfiler*>(userdata);
	that->buffer_.append(ptr, size * nmemb);
	return size * nmemb;
}

// ////////////////////////////////////////////////////////////////////////////////
// int curl_trace(CURL*, curl_infotype type, char* data, size_t size, void*) {
// 	switch (type) {
// 		case CURLINFO_TEXT:
// 			std::cerr << "=> Info: " << data;
// 		default: /* in case a new one is introduced to shock us */
// 			return 0;
// 		case CURLINFO_HEADER_OUT:
// 			std::cerr << "=> Send header: ";
// 			break;
// 		case CURLINFO_DATA_OUT:
// 			std::cerr << "=> Send data: ";
// 			break;
// 		case CURLINFO_SSL_DATA_OUT:
// 			std::cerr << "=> Send SSL data: ";
// 			break;
// 		case CURLINFO_HEADER_IN:
// 			std::cerr << "<= Recv header: ";
// 			break;
// 		case CURLINFO_DATA_IN:
// 			std::cerr << "<= Recv data: ";
// 			break;
// 		case CURLINFO_SSL_DATA_IN:
// 			std::cerr << "<= Recv SSL data: ";
// 			break;
// 	}
// 	if (data) {
// 		std::cerr << std::string(data, size) << "\n";
// 	}
// 	return 0;
// }

////////////////////////////////////////////////////////////////////////////////
std::string get_namespace_prefix(pugi::xml_node node) {
	for (const auto attr : node.attributes()) {
		const auto ns = strstr(attr.name(), "xmlns:");
		if (ns) {
			return std::string(ns + strlen("xmlns:")) + ":";
		}
	}
	return "";
}
