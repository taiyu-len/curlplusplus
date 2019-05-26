#ifndef CURLPLUSPLUS_EASY_INFO_HPP
#define CURLPLUSPLUS_EASY_INFO_HPP
#include <curl/curl.h>   // for CURLINFO*
#include <string>
#include <chrono>

namespace curl {
namespace info { // info types

/** Base template type for options.
 * @param o the option value.
 * @param T the user facing argument type.
 */
template<CURLINFO o, typename T>
struct info {
	using value_type = T;
	T operator()(T x) const noexcept { return x; }
};

template<CURLINFO o>
struct info<o, std::string> {
	using value_type = const char*;
	std::string operator()(const char* x) const noexcept { return x; }
};

template<CURLINFO o>
struct info<o, bool> {
	using value_type = long;
	bool operator()(long x) const noexcept { return static_cast<bool>(x); }
};

template<CURLINFO o, class Rep, class Period>
struct info<o, std::chrono::duration<Rep, Period>> {
	using value_type = curl_off_t;
	std::chrono::duration<Rep, Period> operator()(curl_off_t x) const noexcept
	{
		return std::chrono::duration<Rep, Period>{x};
	}
};

} // namespace info
namespace info { // info objects

#define CURL_INFO_TYPE(NAME, TYPE) info<CURLINFO_##NAME, TYPE>{}
constexpr auto url               = CURL_INFO_TYPE(EFFECTIVE_URL, std::string);
// TODO make response code type that can stringify the response and whatnot.
constexpr auto response_code     = CURL_INFO_TYPE(RESPONSE_CODE, long);
constexpr auto http_connect_code = CURL_INFO_TYPE(HTTP_CONNECTCODE, long);
constexpr auto http_version      = CURL_INFO_TYPE(HTTP_VERSION, long);

constexpr auto redirect_count    = CURL_INFO_TYPE(REDIRECT_COUNT, long);
constexpr auto redirect_url      = CURL_INFO_TYPE(REDIRECT_URL, std::string);
// bytes
constexpr auto size_upload       = CURL_INFO_TYPE(SIZE_UPLOAD_T, curl_off_t);
constexpr auto size_download     = CURL_INFO_TYPE(SIZE_DOWNLOAD_T, curl_off_t);

// bytes per second
constexpr auto speed_upload      = CURL_INFO_TYPE(SPEED_UPLOAD_T, curl_off_t);
constexpr auto speed_download    = CURL_INFO_TYPE(SPEED_DOWNLOAD_T, curl_off_t);
constexpr auto header_size       = CURL_INFO_TYPE(HEADER_SIZE, long);
constexpr auto request_size      = CURL_INFO_TYPE(REQUEST_SIZE, long);

constexpr auto content_length_download = CURL_INFO_TYPE(CONTENT_LENGTH_DOWNLOAD_T, curl_off_t);
constexpr auto content_length_upload   = CURL_INFO_TYPE(CONTENT_LENGTH_UPLOAD_T, curl_off_t);

constexpr auto content_type       = CURL_INFO_TYPE(CONTENT_TYPE, std::string);

// T should be a pointer.
template<typename T>
constexpr auto userdata = CURL_INFO_TYPE(PRIVATE, T);

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

} // namespace info
} // namespace curl
#endif // CURLPLUSPLUS_EASY_INFO_HPP
