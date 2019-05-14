#ifndef CURLPLUSPLUS_MULTI_EVENTS_HPP
#define CURLPLUSPLUS_MULTI_EVENTS_HPP
#include "curl++/buffer.hpp"    // for buffer
#include "curl++/easy_ref.hpp"  // for easy_ref
#include "curl++/types.hpp"
#include <cstddef>
#include <curl/curl.h>
namespace curl {
struct multi_events
{
	// CURLMopt function events
	struct push{};
	struct socket{};
	struct timer{};
};
namespace detail { /* event_fn specializations */
template<typename E> struct event_fn;
template<> struct event_fn<multi_events::push>
{
	template<typename T>
	static int invoke(
		CURL* parent, CURL* easy, size_t num_headers,
		curl_pushheaders *headers, void* userp)
	{
		return static_cast<T*>(userp)->handle(multi_events::push{
		//	parent, easy, num_headers , headers
		});
	}
	using signature = int(CURL*, CURL*, size_t, curl_pushheaders*, void*);
};
template<> struct event_fn<multi_events::socket>
{
	template<typename T>
	static int invoke(
		CURL* easy, curl_socket_t s, int what,
		void* userp, void* socketp)
	{
		return static_cast<T*>(userp)->handle(multi_events::socket{
		//	easy, s, what, socketp
		});
	}
	using signature = int(CURL*, curl_socket_t, void*, void*);
};
template<> struct event_fn<multi_events::timer>
{
	template<typename T>
	static int invoke(CURLM *multi, long timeout_ms, void* userp)
	{
		return static_cast<T*>(userp)->handle(multi_events::timer{
		//	multi, timeout_ms
		});
	}
	using signature = int(CURLM*, long, void*);

};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_EVENTS_HPP
