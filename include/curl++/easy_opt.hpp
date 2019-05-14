#ifndef CURLPLUSPLUS_EASY_OPT_HPP
#define CURLPLUSPLUS_EASY_OPT_HPP
#include "curl++/option.hpp"  // for option_enum, option_base
#include <curl/curl.h>        // for CURLOPT*
#include <string>             // for string
namespace curl {

namespace detail { /* easy_option */

// used to wrap easy option types
template<CURLoption option, typename T>
using easy_option = option_base<CURLoption, option, T>;

// used to construct easy option types with set value.
template<CURLoption option, unsigned long value>
struct easy_option_enum
	: public easy_option<option, unsigned long>
{
	easy_option_enum()
	: easy_option<option, unsigned long>(value)
	{
		// NOOP
	}
};
} // namespace detail
namespace option { /* curl easy options */

//@{
/// Curl option types
#define CURL_OPTION_TYPE(NAME, TYPE) detail::easy_option<CURLOPT_##NAME, TYPE>
#define CURL_ENUM_TYPE(NAME, VALUE)  detail::easy_option_enum<CURLOPT_##NAME, VALUE>
template<typename T>
using state           = CURL_OPTION_TYPE(PRIVATE, T*);
using url             = CURL_OPTION_TYPE(URL, std::string);
using verbose         = CURL_OPTION_TYPE(VERBOSE, bool);
using follow_location = CURL_OPTION_TYPE(FOLLOWLOCATION, bool);
using error_buffer    = CURL_OPTION_TYPE(ERRORBUFFER, curl::error_buffer);
namespace netrc {
	using optional = CURL_ENUM_TYPE(NETRC, CURL_NETRC_OPTIONAL);
	using ignored  = CURL_ENUM_TYPE(NETRC, CURL_NETRC_IGNORED);
	using required = CURL_ENUM_TYPE(NETRC, CURL_NETRC_REQUIRED);
} // namespace netrc
namespace httpauth {
	using basic     = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_BASIC);
	using digest    = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_DIGEST);
	using digest_ie = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_DIGEST_IE);
	using bearer    = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_BEARER);
	using negotiate = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_NEGOTIATE);
	using ntlm      = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_NTLM);
	using ntlm_wb   = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_NTLM_WB);
	using any       = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_ANY);
	using anysafe   = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_ANYSAFE);
	using only      = CURL_ENUM_TYPE(HTTPAUTH, CURLAUTH_ONLY);
} // namespace httpauth
#undef CURL_ENUM_TYPE
#undef CURL_OPTION_TYPE
//@}
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_EASY_OPT_HPP
