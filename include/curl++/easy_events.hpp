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
namespace detail { /* event_info specializations */
template<typename E> struct event_info;
template<typename E> struct fwrite_event
{
	using signature = size_t(void*, size_t, size_t, void*);
	static E get_event(void *d, size_t s, size_t t, void*) noexcept
	{
		return E{buffer{static_cast<char*>(d), s*t}};
	}
	static void* get_dataptr(void *, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

template<> struct event_info<easy_ref::write> : fwrite_event<easy_ref::write> {};
template<> struct event_info<easy_ref::read>  : fwrite_event<easy_ref::read> {};
template<> struct event_info<easy_ref::header>: fwrite_event<easy_ref::header> {};

template<> struct event_info<easy_ref::debug>
{
	using signature = int(CURL*, infotype, char*, size_t, void*);
	static easy_ref::debug get_event(CURL* e, infotype i, char* c, size_t s, void*) noexcept
	{
		return easy_ref::debug{buffer{c, s}, e, i};
	}
	static void* get_dataptr(CURL*, infotype, char*, size_t, void* x) noexcept
	{
		return x;
	}
};

template<> struct event_info<easy_ref::seek>
{
	using signature = int(void*, off_t, int);
	static easy_ref::seek get_event(void*, off_t offset, int origin) noexcept
	{
		return easy_ref::seek{offset, origin};
	}
	static void* get_dataptr(void* x, off_t, int) noexcept
	{
		return x;
	}
};

template<> struct event_info<easy_ref::progress>
{
	using signature = int(void*, off_t, off_t, off_t, off_t);
	static easy_ref::progress get_event(void*, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return easy_ref::progress{dt, dn, ut, un};
	}
	static void* get_dataptr(void* x, off_t, off_t, off_t, off_t) noexcept
	{
		return x;
	}
};
} // option
} // namespace curl
#endif // CURLPLUSPLUS_EASY_EVENTS_HPP
