#ifndef CURLPLUSPLUS_EASY_EVENTS_HPP
#define CURLPLUSPLUS_EASY_EVENTS_HPP
#include "curl++/buffer.hpp"
#include "curl++/easy_ref.hpp"
#include "curl++/option.hpp"
#include "curl++/types.hpp"
namespace curl
{
/* Kinds of events that can be handled */
struct easy_events
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
namespace detail { /* event_fn specializations */
template<> struct event_fn<easy_events::write>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::write{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_events::read>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::read{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_events::header>
{
	template<typename T>
	static size_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::header{buffer{d, s*t}});
	}
	using signature = size_t(char*, size_t, size_t, void*) noexcept;
};
template<> struct event_fn<easy_events::cleanup>
{
	template<typename T>
	static int invoke(void *x) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::cleanup{});
	}
	using signature = int(void*) noexcept;
};
template<> struct event_fn<easy_events::debug>
{
	template<typename T>
	static int invoke(CURL* e, infotype i, char* c, size_t s, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(
			easy_events::debug{buffer{c, s}, e, i});
	}
	using signature = int(CURL*, infotype, char*, size_t, void*) noexcept;
};
template<> struct event_fn<easy_events::seek>
{
	template<typename T>
	static int invoke(void* x, off_t offset, int origin) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::seek{offset, origin});
	}
	using signature = int(void*, off_t, int) noexcept;
};
template<> struct event_fn<easy_events::progress>
{
	template<typename T>
	static int invoke(void* x, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return static_cast<T*>(x)->handle(easy_events::progress{dt, dn, ut, un});
	}
	using signature = int(void*, off_t, off_t, off_t, off_t) noexcept;
};
} // option
} // namespace curl
#endif // CURLPLUSPLUS_EASY_EVENTS_HPP
