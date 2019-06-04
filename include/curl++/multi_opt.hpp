#ifndef CURLPLUSPLUS_MULTI_OPT_HPP
#define CURLPLUSPLUS_MULTI_OPT_HPP
#include "curl++/option.hpp"  // for option_enum, option_base
#include <curl/curl.h>        // for CURLMOPT*
namespace curl {
namespace option { /* Curl multi options */
namespace detail { /* multi_option */

template<CURLMoption option, typename T>
struct multi_option : public option_base<CURLMoption, option, T> {
	using option_base<CURLMoption, option, T>::option_base;
};

/* Enumerated options */
template<CURLMoption option, unsigned long value, typename T = unsigned long>
struct multi_option_enum : public multi_option<option, T> {
	multi_option_enum()
	: multi_option<option, T>(value)
	{}
};

} // namespace detail

/// Curl option types
#define CURLM_OPTION_TYPE(NAME, TYPE) detail::multi_option<CURLMOPT_##NAME, TYPE>
#define CURLM_ENUM_TYPE(NAME, VALUE)  detail::multi_option_enum<CURLMOPT_##NAME, VALUE>
#define CURLM_FLAG_TYPE(NAME, VALUE)  detail::multi_option_enum<CURLMOPT_##NAME, VALUE, detail::bit_flag_option>
using max_host_connections  = CURLM_OPTION_TYPE(MAX_HOST_CONNECTIONS, long);
using max_total_connections = CURLM_OPTION_TYPE(MAX_TOTAL_CONNECTIONS, long);
using max_connects          = CURLM_OPTION_TYPE(MAXCONNECTS, long);
namespace pipelining {
	using nothing   = CURLM_FLAG_TYPE(PIPELINING, CURLPIPE_NOTHING);
	using HTTP1     = CURLM_FLAG_TYPE(PIPELINING, CURLPIPE_HTTP1);
#ifdef CURLPIPE_MULTIPLEX
	using MULITPLEX = CURLM_FLAG_TYPE(PIPELINING, CURLPIPE_MULTIPLEX);
#endif // CURLPIPE_MULTIPLEX
} // namespace pipelining
#undef CURLM_FLAG_TYPE
#undef CURLM_ENUM_TYPE
#undef CURLM_OPTION_TYPE
//@}
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_OPT_HPP
