#ifndef CURLPLUSPLUS_GLOBAL_HPP
#define CURLPLUSPLUS_GLOBAL_HPP
#include "curl++/invoke.hpp"
#include <curl/curl.h>
namespace curl {
/**
 * RAII wrapper around curl global state.
 */
struct global {
	/** Bit flags used to initialize curl global state. */
	enum flags : unsigned long {
		ALL        = CURL_GLOBAL_ALL,
		SSL        = CURL_GLOBAL_SSL,
		WIN32      = CURL_GLOBAL_WIN32,
		NOTHING    = CURL_GLOBAL_NOTHING,
		DEFAULT    = CURL_GLOBAL_DEFAULT,
		ACK_EINTER = CURL_GLOBAL_ACK_EINTR
	};

	/** Initialize curl global state with given flags.
	 *
	 * @throws curl::code if initialization failed
	 */
	explicit global(flags f = DEFAULT)
	{
		invoke(curl_global_init, f);
	}

	/** Cleans up curl global state. */
	~global() noexcept
	{
		curl_global_cleanup();
	}
};
} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
