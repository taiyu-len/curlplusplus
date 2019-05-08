#ifndef CURLPLUSPLUS_EASY_INFO_HPP
#define CURLPLUSPLUS_EASY_INFO_HPP
#include <curl/curl.h> // for CURLINFO*
#include <string>

namespace curl {
namespace info {
/** Base template type for options.
 * @param o the option value.
 * @param T the user facing argument type.
 */
template<CURLINFO o, typename T>
struct info
{
	using value_type = T;
	T operator()(T x) { return x; }
};
template<CURLINFO o>
struct info<o, std::string>
{
	using value_type = const char*;
	std::string operator()(const char* x) { return x; }
};
template<CURLINFO o>
struct info<o, bool>
{
	using value_type = long;
	long operator()(bool x) { return static_cast<long>(x); }
};


//@{
/// Curl info types
#define CURL_INFO_TYPE(NAME, TYPE) info<CURLINFO_##NAME, TYPE>
using url = CURL_INFO_TYPE(EFFECTIVE_URL, std::string);
using response_code = CURL_INFO_TYPE(RESPONSE_CODE, long); // TODO make response code type
using content_type = CURL_INFO_TYPE(CONTENT_TYPE, std::string);
#undef CURL_INFO_TYPE
//@}
} // namespace info
} // namespace curl
#endif // CURLPLUSPLUS_EASY_INFO_HPP
