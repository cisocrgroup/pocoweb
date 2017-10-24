#include "RemoteProfilerTemplate.hpp"
#include <cassert>
#include <cstring>
#include "utils/Base64.hpp"

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
const char* RemoteProfilerTemplate::t =
    R"(
<?xml version = "1.0" encoding='utf-8'?>
<ns1:GetProfileRequest xmlns:ns1='http://www.cis.uni-muenchen.de/profiler/'>
		<ns1:userid>__USERID__</ns1:userid>
		<ns1:configuration>__LANGUAGE__</ns1:configuration>
		<ns1:doc_in>
				<!-- optional -->
				<ns1:fileName>__FILE_NAME__</ns1:fileName>
				<!-- optional -->
				<ns1:binaryData>__DATA__</ns1:binaryData>
		</ns1:doc_in>
		<ns1:doc_in_type>DOCXML</ns1:doc_in_type>
		<ns1:doc_in_size>__SIZE__</ns1:doc_in_size>
</ns1:GetProfileRequest>)";

////////////////////////////////////////////////////////////////////////////////
RemoteProfilerTemplate& RemoteProfilerTemplate::set_userid(
    const std::string& userid) {
	return set_string("__USERID__", strlen("__USERID__"), userid);
}

////////////////////////////////////////////////////////////////////////////////
RemoteProfilerTemplate& RemoteProfilerTemplate::set_filename(
    const std::string& filename) {
	return set_string("__FILE_NAME__", strlen("__FILE_NAME__"), filename);
}
////////////////////////////////////////////////////////////////////////////////
RemoteProfilerTemplate& RemoteProfilerTemplate::set_language(
    const std::string& language) {
	return set_string("__LANGUAGE__", strlen("__LANGUAGE__"), language);
}
////////////////////////////////////////////////////////////////////////////////
RemoteProfilerTemplate& RemoteProfilerTemplate::set_data(
    const std::string& data) {
	const auto b64 = base64::encode(data);
	const auto size = std::to_string(b64.size());
	set_string("__DATA__", strlen("__DATA__"), b64);
	set_string("__SIZE__", strlen("__SIZE__"), size);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
RemoteProfilerTemplate& RemoteProfilerTemplate::set_string(
    const char* key, size_t n, const std::string& str) {
	if (data_.empty()) {
		data_ = t;
	}
	const auto pos = data_.find(key);
	assert(pos != std::string::npos);
	data_.replace(pos, n, str);
	return *this;
}
