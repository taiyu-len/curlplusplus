#ifndef CURLPLUSPLUS_EASY_INFO_HPP
#define CURLPLUSPLUS_EASY_INFO_HPP
#include <chrono>
#include <curl/curl.h>   // for CURLINFO*
#include <string>

namespace curl {
namespace info {
namespace detail {
/**
 * Base info type.
 * @param 1 the info value.
 * @param T the user facing argument type.
 */
template<CURLINFO, typename T>
struct info {
	// C-API compatible value type.
	static auto value() -> T { return T{}; }

	// C-API -> C++ API conversion function
	static auto convert(T x) -> T
	{
		return x;
	}
};

template<CURLINFO I>
struct info<I, std::string> {
	static auto value() -> const char* { return {}; }
	static auto convert(const char* x) -> std::string
	{
		return x;
	}
};

template<CURLINFO I>
struct info<I, bool> {
	static auto value() -> long { return {}; }
	static auto convert(long x) -> bool
	{
		return static_cast<bool>(x);
	}
};

template<CURLINFO I, class Rep, class Period>
struct info<I, std::chrono::duration<Rep, Period>> {
	static auto value() -> curl_off_t { return {}; }
	static auto convert(curl_off_t x) -> std::chrono::duration<Rep, Period>
	{
		return std::chrono::duration<Rep, Period>{x};
	}
};
} // namespace detail

/* Declaration of info variables.
 * Formatting command.
s/(\(.*\), \(.*\), \(.*\))/\=printf("(%-24, %-24, %s)", submatch(1), submatch(2), submatch(3))
 */
#define CURL_INFO_VAR(NAME, CODE, TYPE) \
constexpr auto NAME = detail::info<CURLINFO_##CODE, TYPE>{}
CURL_INFO_VAR(url                     , EFFECTIVE_URL           , std::string);
// TODO make response code type that can stringify the response and whatnot.
CURL_INFO_VAR(response_code           , RESPONSE_CODE           , long);
CURL_INFO_VAR(http_connect_code       , HTTP_CONNECTCODE        , long);
CURL_INFO_VAR(http_version            , HTTP_VERSION            , long);

CURL_INFO_VAR(redirect_count          , REDIRECT_COUNT          , long);
CURL_INFO_VAR(redirect_url            , REDIRECT_URL            , std::string);
// bytes
CURL_INFO_VAR(size_upload             , SIZE_UPLOAD_T           , curl_off_t);
CURL_INFO_VAR(size_download           , SIZE_DOWNLOAD_T         , curl_off_t);
// bytes per second
CURL_INFO_VAR(speed_upload            , SPEED_UPLOAD_T          , curl_off_t);
CURL_INFO_VAR(speed_download          , SPEED_DOWNLOAD_T        , curl_off_t);
CURL_INFO_VAR(header_size             , HEADER_SIZE             , long);
CURL_INFO_VAR(request_size            , REQUEST_SIZE            , long);
// Bytes
CURL_INFO_VAR(content_length_download , CONTENT_LENGTH_DOWNLOAD_T, curl_off_t);
CURL_INFO_VAR(content_length_upload   , CONTENT_LENGTH_UPLOAD_T , curl_off_t);
CURL_INFO_VAR(content_type            , CONTENT_TYPE            , std::string);
// T should be a pointer.
template<typename T>
CURL_INFO_VAR(userdata                , PRIVATE                 , T);
// TODO rather then a duration, a timepoint may be better
CURL_INFO_VAR(filetime                , FILETIME_T              , std::chrono::seconds);
// Timings
CURL_INFO_VAR(total_time              , TOTAL_TIME_T            , std::chrono::microseconds);
CURL_INFO_VAR(namelookup_time         , NAMELOOKUP_TIME_T       , std::chrono::microseconds);
CURL_INFO_VAR(connect_time            , CONNECT_TIME_T          , std::chrono::microseconds);
CURL_INFO_VAR(appconnect_time         , APPCONNECT_TIME_T       , std::chrono::microseconds);
CURL_INFO_VAR(pretransfer_time        , PRETRANSFER_TIME_T      , std::chrono::microseconds);
CURL_INFO_VAR(starttransfer_time      , STARTTRANSFER_TIME_T    , std::chrono::microseconds);
CURL_INFO_VAR(redirect_time           , REDIRECT_TIME_T         , std::chrono::microseconds);
#undef CURL_INFO_VAR

} // namespace info
} // namespace curl
#endif // CURLPLUSPLUS_EASY_INFO_HPP
