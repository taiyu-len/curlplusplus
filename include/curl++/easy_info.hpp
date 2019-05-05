#ifndef CURLPLUSPLUS_EASY_INFO_HPP
#define CURLPLUSPLUS_EASY_INFO_HPP
#include "curl++/util.hpp"
#include <string>
#include <utility>

namespace curl {
namespace info {
/** Base template type for options.
 * @param o the option value.
 * @param T the user facing argument type.
 */
template<CURLINFO o, typename T>
struct info : private detail::translate<T>
{
	using typename detail::translate<T>::outer_t;
	using typename detail::translate<T>::inner_t;
	using detail::translate<T>::to_outer;
};

//@{
/// Curl info types
#define CURL_INFO_TYPE(NAME, TYPE) info<CURLINFO_##NAME, TYPE>
using url = CURL_INFO_TYPE(EFFECTIVE_URL, std::string);

#undef CURL_INFO_TYPE
//@}
} // namespace info
} // namespace curl
#endif // CURLPLUSPLUS_EASY_INFO_HPP
