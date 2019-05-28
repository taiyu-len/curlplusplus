#ifndef CURLPLUSPLUS_DETAIL_MULTI_EVENTS_HPP
#define CURLPLUSPLUS_DETAIL_MULTI_EVENTS_HPP
#include "curl++/multi.hpp"
namespace curl {
// TODO implement
struct multi_ref::push {
	using signature = int(CURL*, CURL*, size_t, curl_pushheaders*, void*);

	static constexpr CURLMoption FUNC = CURLMOPT_PUSHFUNCTION;
	static constexpr CURLMoption DATA = CURLMOPT_PUSHDATA;

	push(CURL*, CURL*, size_t, curl_pushheaders*, void*)
	{}

	static void* dataptr(CURL*, CURL*, size_t, curl_pushheaders*, void* x) noexcept
	{
		return x;
	}
};

struct multi_ref::socket {
	using signature = int(CURL*, curl_socket_t, int, void*, void*);

	static constexpr CURLMoption FUNC = CURLMOPT_SOCKETFUNCTION;
	static constexpr CURLMoption DATA = CURLMOPT_SOCKETDATA;

	enum poll
	{
		in     = CURL_POLL_IN,
		out    = CURL_POLL_OUT,
		inout  = CURL_POLL_INOUT,
		remove = CURL_POLL_REMOVE
	};

	easy_ref      easy;
	curl_socket_t sock;
	poll          what;
	void*         data;

	socket(CURL* e, curl_socket_t s, int w, void*, void* d) noexcept
	: easy(e)
	, sock(s)
	, what(static_cast<poll>(w))
	, data(d)
	{}

	static void* dataptr(CURL*, curl_socket_t, int, void* x, void*) noexcept
	{
		return x;
	}
};

struct multi_ref::timer {
	using signature = int(CURLM*, long, void*);

	static constexpr CURLMoption FUNC = CURLMOPT_TIMERFUNCTION;
	static constexpr CURLMoption DATA = CURLMOPT_TIMERDATA;

	multi_ref                 multi;
	std::chrono::milliseconds timeout;

	timer(CURLM* m, long timeout_ms, void*) noexcept
	: multi(m)
	, timeout(timeout_ms)
	{}

	static void* dataptr(CURLM*, long, void* x) noexcept
	{
		return x;
	}
};

} // namespace curl
#endif // CURLPLUSPLUS_DETAIL_MULTI_EVENTS_HPP
