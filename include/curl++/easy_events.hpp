#ifndef CURLPLUSPLUS_EASY_EVENTS_HPP
#define CURLPLUSPLUS_EASY_EVENTS_HPP
#include "curl++/buffer.hpp"    // for buffer
#include "curl++/easy_ref.hpp"  // for easy_ref
#include "curl++/types.hpp"     // for off_t infotype
#include <cstddef>              // for size_t
#include <curl/curl.h>          // for CURL, CURL_WRITEFUNC_PAUSE
namespace curl
{
/* Kinds of events that can be handled */
struct easy_ref::debug : buffer
{
	debug(buffer b, easy_ref h, infotype i)
		: buffer{b}, handle{h}, type{i} {}
	easy_ref handle;
	infotype type;
};
struct easy_ref::header : buffer
{
	explicit header(buffer b) : buffer{b} {}
};
struct easy_ref::read : buffer
{
	explicit read(buffer b):buffer{b} {}
};
struct easy_ref::seek
{
	off_t offset; int origin;
};
struct easy_ref::write : buffer
{
	explicit write(buffer b):buffer{b} {}
	static constexpr size_t pause = CURL_WRITEFUNC_PAUSE;
};
struct easy_ref::progress
{
	off_t dltotal, dlnow, ultotal, ulnow;
};
;
namespace detail { /* event_fn specializations */
template<typename E> struct event_fn;
template<typename E> struct fwrite_event
{
	template<typename T>
	using signature = size_t(char*, size_t, size_t, T*);

	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		auto ev = E(buffer{d, s*t});
		return static_cast<T*>(x)->handle(ev);
	}
};

template<> struct event_fn<easy_ref::write> : fwrite_event<easy_ref::write> {};
template<> struct event_fn<easy_ref::read>  : fwrite_event<easy_ref::read> {};
template<> struct event_fn<easy_ref::header>: fwrite_event<easy_ref::header> {};

template<> struct event_fn<easy_ref::debug>
{
	template<typename T>
	using signature = int(CURL*, infotype, char*, size_t, T*);

	template<typename T>
	static int invoke(CURL* e, infotype i, char* c, size_t s, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(
			easy_ref::debug{buffer{c, s}, e, i});
	}
};
template<> struct event_fn<easy_ref::seek>
{
	template<typename T>
	using signature = int(T*, off_t, int);

	template<typename T>
	static int invoke(void* x, off_t offset, int origin) noexcept
	{
		return static_cast<T*>(x)->handle(easy_ref::seek{offset, origin});
	}
};
template<> struct event_fn<easy_ref::progress>
{
	template<typename T>
	using signature = int(T*, off_t, off_t, off_t, off_t);

	template<typename T>
	static int invoke(void* x, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return static_cast<T*>(x)->handle(easy_ref::progress{dt, dn, ut, un});
	}
};
} // option
} // namespace curl
#endif // CURLPLUSPLUS_EASY_EVENTS_HPP
