#include "RemoteProfiler.hpp"
#include <crow/logging.h>
#include <curl/curl.h>
#include "Profile.hpp"
#include "pugixml.hpp"
#include "utils/Error.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
Profile RemoteProfiler::do_profile() {
	THROW(NotImplemented, "RemoteProfiler::do_profile");
}
#pragma GCC diagnostic pop

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> RemoteProfiler::do_languages() {
	const auto path = boost::filesystem::path(url_) / "getConfigurations";
	struct curl_clean {
		void operator()(void* curl) const noexcept {
			curl_easy_cleanup(curl);
		}
	};
	std::unique_ptr<CURL, curl_clean> curl(curl_easy_init());
	if (not curl) THROW(Error, "could not intialize curl");
	char EBUF[CURL_ERROR_SIZE + 1];
	buffer_.clear();
	// curl_easy_setopt(curl.get(), CURLOPT_PORT, port);
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
	auto nodes =
	    xml.document_element().select_nodes(".//ns2:configurations");
	std::vector<std::string> languages;
	for (const auto& node : nodes) {
		languages.push_back(node.node().child_value());
	}
	return languages;
}

// ////////////////////////////////////////////////////////////////////////////////
// size_t RemoteProfiler::read(void *ptr, size_t size, size_t nmemb, void
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
