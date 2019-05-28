#ifndef CURLPLUSPLUS_TYPES_HPP
#define CURLPLUSPLUS_TYPES_HPP
#include <curl/curl.h>   // for CURLMcode, CURLSHcode, CURLUcode, CURLcode
#include <exception>     // for exception
#include <string>        // for string
namespace curl {

// simple typedefs of curl types
using infotype = curl_infotype;
using socket_t = curl_socket_t;

// error buffer type
struct error_buffer : public std::string {
	error_buffer()
	: std::string(CURL_ERROR_SIZE, '\0')
	{};
};

namespace detail {

template<typename T, T OK, const char*(*to_string)(T)>
struct code_template : std::exception {
	T value = OK;

	code_template(T value) noexcept
	: value(value)
	{}

	explicit operator bool() const noexcept
	{
		return value != OK;
	}

	const char* what() const noexcept override
	{
		return to_string(value);
	}
};

// curl doesnt provide this for some reason.
inline const char* url_strerror(CURLUcode value) {
	switch (value) {
	case CURLUE_BAD_HANDLE:
		return "An argument that should be a CURLU pointer was passed in as a NULL.";
	case CURLUE_BAD_PARTPOINTER:
		return "A NULL pointer was passed to the 'part' argument of curl_url_get(3).";
	case CURLUE_MALFORMED_INPUT:
		return "A malformed input was passed to a URL API function.";
	case CURLUE_BAD_PORT_NUMBER:
		return "The port number was not a decimal number between 0 and 65535.";
	case CURLUE_UNSUPPORTED_SCHEME:
		return "This libcurl build doesn't support the given URL scheme.";
	case CURLUE_URLDECODE:
		return "URL decode error, most likely because of rubbish in the input.";
	case CURLUE_OUT_OF_MEMORY:
		return "A memory function failed.";
	case CURLUE_USER_NOT_ALLOWED:
		return "Credentials was passed in the URL when prohibited.";
	case CURLUE_UNKNOWN_PART:
		return "An unknown part ID was passed to a URL API function.";
	case CURLUE_NO_SCHEME:
		return "There is no scheme part in the URL.";
	case CURLUE_NO_USER:
		return "There is no user part in the URL.";
	case CURLUE_NO_PASSWORD:
		return "There is no password part in the URL.";
	case CURLUE_NO_OPTIONS:
		return "There is no options part in the URL.";
	case CURLUE_NO_HOST:
		return "There is no host part in the URL.";
	case CURLUE_NO_PORT:
		return "There is no port part in the URL.";
	case CURLUE_NO_QUERY:
		return "There is no query part in the URL.";
	case CURLUE_NO_FRAGMENT:
		return "There is no fragment part in the URL.";
	case CURLUE_OK:
		return "No url error.";
	default:
		return "Unknown url error.";
	}
}

} // namespace detail

using code   = detail::code_template< CURLcode  , CURLE_OK  , &curl_easy_strerror  >;
using mcode  = detail::code_template< CURLMcode , CURLM_OK  , &curl_multi_strerror >;
using shcode = detail::code_template< CURLSHcode, CURLSHE_OK, &curl_share_strerror >;
using ucode  = detail::code_template< CURLUcode , CURLUE_OK , &detail::url_strerror>;

inline auto to_code(CURLcode c)   noexcept -> code   { return c; }
inline auto to_code(CURLMcode c)  noexcept -> mcode  { return c; }
inline auto to_code(CURLSHcode c) noexcept -> shcode { return c; }
inline auto to_code(CURLUcode c)  noexcept -> ucode  { return c; }

} // namespace curl

#endif // CURLPLUSPLUS_TYPES_HPP

