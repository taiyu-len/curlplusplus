#include <curl++/types.hpp>
namespace curl {

error_buffer::error_buffer()
: std::string(CURL_ERROR_SIZE, '\0')
{
	// NOOP
}

const char* code::what() const noexcept
{
	return curl_easy_strerror(value);
}
code::operator bool() const noexcept
{
	return value != CURLE_OK;
}

const char* mcode::what() const noexcept
{
	return curl_multi_strerror(value);
}
mcode::operator bool() const noexcept
{
	return value != CURLM_OK;
}

const char* shcode::what() const noexcept
{
	return curl_share_strerror(value);
}
shcode::operator bool() const noexcept
{
	return value != CURLSHE_OK;
}

const char* ucode::what() const noexcept
{
	// curl doesnt have a strerror for this.
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
	default:
		return "No url error.";
	}
}
ucode::operator bool() const noexcept
{
	return value != CURLUE_OK;
}

} // namespace curl
