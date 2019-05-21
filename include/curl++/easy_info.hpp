#ifndef CURLPLUSPLUS_EASY_INFO_HPP
#define CURLPLUSPLUS_EASY_INFO_HPP
#include "curl++/types.hpp"
#include <curl/curl.h> // for CURLINFO*
#include <string>
#include <chrono>

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
	T operator()(T x) const noexcept { return x; }
};

template<CURLINFO o>
struct info<o, std::string>
{
	using value_type = const char*;
	std::string operator()(const char* x) const noexcept { return x; }
};

template<CURLINFO o>
struct info<o, bool>
{
	using value_type = long;
	bool operator()(long x) const noexcept { return static_cast<bool>(x); }
};

template<CURLINFO o, class Rep, class Period>
struct info<o, std::chrono::duration<Rep, Period>>
{
	using value_type = curl_off_t;
	std::chrono::duration<Rep, Period> operator()(curl_off_t x) const noexcept
	{
		return std::chrono::duration<Rep, Period>{x};
	}
};

//@{
/// Curl info types
#define CURL_INFO_TYPE(NAME, TYPE) info<CURLINFO_##NAME, TYPE>{}
constexpr auto url               = CURL_INFO_TYPE(EFFECTIVE_URL, std::string);
// TODO make response code type that can stringify the response and whatnot.
constexpr auto response_code     = CURL_INFO_TYPE(RESPONSE_CODE, long);
constexpr auto http_connect_code = CURL_INFO_TYPE(HTTP_CONNECTCODE, long);
constexpr auto http_version      = CURL_INFO_TYPE(HTTP_VERSION, long);
constexpr auto content_type      = CURL_INFO_TYPE(CONTENT_TYPE, std::string);

// TODO rather then a duration, a timepoint may be better
constexpr auto filetime           = CURL_INFO_TYPE(FILETIME_T, std::chrono::seconds);

constexpr auto total_time         = CURL_INFO_TYPE(TOTAL_TIME_T,         std::chrono::microseconds);
constexpr auto namelookup_time    = CURL_INFO_TYPE(NAMELOOKUP_TIME_T,    std::chrono::microseconds);
constexpr auto connect_time       = CURL_INFO_TYPE(CONNECT_TIME_T,       std::chrono::microseconds);
constexpr auto appconnect_time    = CURL_INFO_TYPE(APPCONNECT_TIME_T,    std::chrono::microseconds);
constexpr auto pretransfer_time   = CURL_INFO_TYPE(PRETRANSFER_TIME_T,   std::chrono::microseconds);
constexpr auto starttransfer_time = CURL_INFO_TYPE(STARTTRANSFER_TIME_T, std::chrono::microseconds);
constexpr auto redirect_time      = CURL_INFO_TYPE(REDIRECT_TIME_T,      std::chrono::microseconds);
#undef CURL_INFO_TYPE
//@}
} // namespace info
} // namespace curl
#endif // CURLPLUSPLUS_EASY_INFO_HPP
