#ifndef CURLPLUSPLUS_EASY_OPT_HPP
#define CURLPLUSPLUS_EASY_OPT_HPP
#include "curl++/util.hpp"
#include "curl++/types.hpp"
#include <utility>
#include <string>

namespace curl {
/** Flags used to set netrc easyopt. */
enum class netrc : long
{
	optional = CURL_NETRC_OPTIONAL,
	ignored  = CURL_NETRC_IGNORED,
	required = CURL_NETRC_REQUIRED
};

/** Flags used to set httpauth easyopt. */
enum httpauth : unsigned long
{
	basic     = CURLAUTH_BASIC,
	digest    = CURLAUTH_DIGEST,
	digest_ie = CURLAUTH_DIGEST_IE,
	bearer    = CURLAUTH_BEARER,
	negotiate = CURLAUTH_NEGOTIATE,
	ntlm      = CURLAUTH_NTLM,
	ntlm_wb   = CURLAUTH_NTLM_WB,
	any       = CURLAUTH_ANY,
	anysafe   = CURLAUTH_ANYSAFE,
	only      = CURLAUTH_ONLY,
};

namespace option {
/** Base template type for options.
 * @param o the option value.
 * @param T the user facing argument type.
 */
template<CURLoption o, typename T>
struct option : private detail::translate<T>
{
	using typename detail::translate<T>::outer_t;
	using typename detail::translate<T>::inner_t;
	explicit option(outer_t x): value(this->to_inner(x)) {};
	/** restrict constructing option from inner type unless permitted.
	 * currently only std::string permits creating option type from const
	 * char*
	 */
	template<
		typename S = T,
		typename = std::enable_if_t<detail::translate<T>::allow_inner>>
	explicit option(inner_t x): value(x) {};
	inner_t value;
};

//@{
/// Curl option types
#define CURL_OPTION_TYPE(NAME, TYPE) option<CURLOPT_##NAME, TYPE>
using url = CURL_OPTION_TYPE(URL, std::string);

#undef CURL_OPTION_TYPE
//@}
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_EASY_OPT_HPP
