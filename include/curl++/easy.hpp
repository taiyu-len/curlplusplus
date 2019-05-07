#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "curl++/buffer.hpp"
#include "curl++/easy_handle.hpp"
#include "curl++/extract_function.hpp"
#include <type_traits>
namespace curl {
/* Kinds of events that can be handled */
struct easy_event
{
	struct cleanup {};
	struct debug : buffer
	{
		debug(buffer b, easy_ref h, infotype i)
		: buffer{b}, handle{h}, info{i} {}
		easy_ref handle;
		infotype info;
	};
	struct header : buffer
	{
		explicit header(buffer b) : buffer{b} {}
	};
	struct read : buffer
	{
		explicit read(buffer b):buffer{b} {}
	};
	struct seek
	{
		off_t offset; int origin;
	};
	struct write : buffer
	{
		explicit write(buffer b):buffer{b} {}
		static constexpr size_t pause = CURL_WRITEFUNC_PAUSE;
	};
	struct progress
	{
		off_t dltotal, dlnow, ultotal, ulnow;
	};
};

/** CRTP easy handle that automatically sets callbacks and data.
 * @param T The parent type.
 *
 * callbacks are conditionally set in the constructor based on what overloads
 * of the function `handle` exist in T.
 * each overload handles a different event.
 * - event::cleanup
 *   responsible for managing the lifetime of the object itself, after this
 *   is called, the object should be assumed dead.
 * - event::debug
 *   called by libcurl, see CURLOPT_DEBUGFUNCTION
 * - event::header
 *   called by libcurl, see CURLOPT_HEADERFUNCTION
 * - event::progress
 *   called by libcurl, see CURLOPT_XFERINFOFUNCTION
 * - event::read
 *   called by libcurl, see CURLOPT_READFUNCTION
 * - event::write
 *   called by libcurl, see CURLOPT_WRITEFUNCTION
 *
 * TODO add more handlers for CURLOPT_*FUNCTION's
 *
 * example usage.
 * \code{.cpp}
struct printer : curl::easy<printer> {
	// the function that will be used for write_function callback
	size_t handle(event::write) noexcept {
		// print data received
	}
	int handle(event::cleanup) noexcept {
		// print footer after data received.
	}
};
\endcode
 *
 */
template<typename T>
struct easy
	: public easy_handle
	, public easy_event
{
	easy()
	{
		auto self = static_cast<T*>(this);
		// set event handlers conditionally.
		set_handler(detail::extract_fn< cleanup  >(self));
		set_handler(detail::extract_fn< debug    >(self));
		set_handler(detail::extract_fn< header   >(self));
		set_handler(detail::extract_fn< read     >(self));
		set_handler(detail::extract_fn< seek     >(self));
		set_handler(detail::extract_fn< write    >(self));
		set_handler(detail::extract_fn< progress >(self));
	}
	void handle(curl::code) const noexcept {};
private:
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};

namespace detail { /* event_fn for easy events */
// write handler
template<> struct event_fn<easy_event::write>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::write{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_event::read>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::read{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_event::header>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::header{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_event::cleanup>
{
	template<typename T>
	static int invoke(void *x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::cleanup{});
	}
	using signature = int(void*) noexcept;
};
template<> struct event_fn<easy_event::debug>
{
	template<typename T>
	static int invoke(CURL* e, infotype i, char* c, size_t s, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(
			easy_event::debug{buffer{c, s}, e, i});
	}
	using signature = int(CURL*, infotype, char*, size_t, void*) noexcept;
};
template<> struct event_fn<easy_event::seek>
{
	template<typename T>
	static int invoke(void* x, off_t offset, int origin) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::seek{offset, origin});
	}
	using signature = int(void*, off_t, int) noexcept;
};
template<> struct event_fn<easy_event::progress>
{
	template<typename T>
	static int invoke(void* x, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return static_cast<T*>(x)->handle(easy_event::progress{dt, dn, ut, un});
	}
	using signature = int(void*, off_t, off_t, off_t, off_t) noexcept;
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
