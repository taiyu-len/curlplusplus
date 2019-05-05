#ifndef CURLPLUSPLUS_GLOBAL_HPP
#define CURLPLUSPLUS_GLOBAL_HPP
#include <curl/curl.h>
namespace curl {
/** RAII wrapper around curl global state.
 *
 * see curl_global_init, curl_global_cleanup
 */
struct global
{
	/** Bit flags used to initialize curl global state. */
	enum flags : unsigned long
	{
		ALL        = CURL_GLOBAL_ALL,
		SSL        = CURL_GLOBAL_SSL,
		WIN32      = CURL_GLOBAL_WIN32,
		NOTHING    = CURL_GLOBAL_NOTHING,
		DEFAULT    = CURL_GLOBAL_DEFAULT,
		ACK_EINTER = CURL_GLOBAL_ACK_EINTR
	};

	//@{
	/** Initialize curl global state with given flags.
	 *
	 * @param f flags passed to curl_global_init
	 * @throws std::runtime_error if initialization failed
	 */
	global(flags f);
	global();
	//@}

	/** Cleans up curl global state. */
	~global() noexcept;
};
} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
