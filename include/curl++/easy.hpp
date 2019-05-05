#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "easy_t.hpp"
#include <type_traits>
#include <curl/curl.h>
namespace curl {

/** CRTP easy handle that automatically sets callbacks and data.
 * callbacks are set in constructor based on what member functions exist in
 * parent type.
 *
 * callbacks used are
 * - cleanup: Called when an easy request is completed to cleanup state.
 * - debug: CURLOPT_DEBUGFUNCTION
 * - header: CURLOPT_HEADERFUNCTION
 * - progress: CURLOPT_XFERINFOFUNCTION
 * - read:  CURLOPT_READFUNCTION
 * - write: CURLOPT_WRITEFUNCTION
 *
 * static versions of these callbacks allow the user to set the DATA pointer for
 * that callback if the type matches.
 * multiple static versions can exist and one can set the callback by setting
 * the data.
 *
 * example usage.
 * \code{.cpp}
struct printer : curl::easy<printer> {
	// the function that will be used for write_function callback
	size_t write(char* data, size_t length) noexcept {
		// print data received
	}
	int cleanup() noexcept {
		// print footer after data received.
	}
};
\endcode
 */
template<typename T>
struct easy
	: public easy_t
{
};

} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
