#ifndef CURLPLUSPLUS_EVENT_HPP
#define CURLPLUSPLUS_EVENT_HPP
#include <cstddef>
#include <curl/curl.h>
#include <type_traits>
#include "curl++/easy_ref.hpp"
#include "curl++/types.hpp"
namespace curl {
// dumb buffer type. TODO replace with fancy span like type
struct buffer_t {
	char* data;
	size_t size;
};

/* easy event types */
namespace event {
// TODO fancier types with convenient functions
// consider better 
struct cleanup {};
struct debug   : buffer_t
{
	debug(buffer_t b, easy_ref h, infotype i)
	: buffer_t{b}, handle{h}, info{i} {}
	easy_ref handle;
	infotype info;
};
struct header  : buffer_t { explicit header(buffer_t b):buffer_t{b} {} };
struct read    : buffer_t { explicit read(buffer_t b):buffer_t{b} {} };
struct seek    { off_t offset; int origin; };
struct write   : buffer_t {
	explicit write(buffer_t b):buffer_t{b} {}
	static constexpr size_t pause = CURL_WRITEFUNC_PAUSE;
};
struct progress{ off_t dltotal, dlnow, ultotal, ulnow; };
} // namespace event

//@{
/** Event signatures used to match agains */
namespace detail {
template<class E> struct event_info
{
	using return_t = size_t;
	using signature = size_t(char *, size_t, size_t, void*);
	template<typename T>
	static return_t invoke(char *d, size_t s, size_t t, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(E{buffer_t{d, s*t}});
	}
};
template<> struct event_info<event::cleanup>
{
	using return_t = int;
	using signature = int(void*);
	template<typename T>
	static return_t invoke(void *x) noexcept {
		return static_cast<T*>(x)->handle(event::cleanup{});
	}
};
template<> struct event_info<event::debug>
{
	using return_t = int;
	using signature = int(CURL*, infotype, char*, size_t, void*);
	template<typename T>
	static return_t invoke(CURL* e, infotype i, char* c, size_t s, void* x) noexcept
	{
		return static_cast<T*>(x)->handle(
			event::debug{buffer_t{c, s}, e, i});
	}
};
template<> struct event_info<event::seek>
{
	using return_t = int;
	using signature = int(void*, off_t, int);
	template<typename T>
	static return_t invoke(void* x, off_t offset, int origin) noexcept
	{
		return static_cast<T*>(x)->handle(event::seek{offset, origin});
	}
};
template<> struct event_info<event::progress>
{
	using return_t = int;
	using signature = int(void*, off_t, off_t, off_t, off_t);
	template<typename T>
	static return_t invoke(void* x, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return static_cast<T*>(x)->handle(event::progress{dt, dn, ut, un});
	}
};
} // namespace detail
//@}
//@{
/** Gets the member function pointer for the event from T */
namespace detail {
template<typename E, typename T, typename = void>
struct mem_fn
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept {
		return nullptr;
	}
};

template<typename E, typename T>
struct mem_fn<E, T, decltype(std::declval<T>().handle(std::declval<E>()), void())>
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept
	{
		return &event_info<E>::template invoke<T>;
	}
};
} // namespace detail
//@}
//@{
/** Argument type that sets the function pointer on construction */
namespace detail {
template<class event>
struct callback
{
	/// Construct data from argument, and function pointer from a member
	/// function of T if it exists, otherwise make both nullptr.
	template<typename T>
	callback(T* x) noexcept
	: fptr(detail::mem_fn<event, T>::fptr())
	, data(static_cast<void*>(x))
	{ /* NOOP */ }

	/// Construct data from D* argument, and function pointer from static
	/// member function of T
	template<typename T, typename D>
	callback(T*, D* x) noexcept
	: fptr(/* TODO detail::bar<S, T>::fptr() */)
	, data(x)
	{ /* NOOP */ }

	// sets the callback on the raw handle
	void set_handler(CURL *) const noexcept;
private:
	// type erased function pointer taking void*
	typename event_info<event>::signature* fptr;
	// type erased data passed to
	void* data;
};

} // namespace detail
//@}
} // namespace curl

#endif // CURLPLUSPLUS_EVENT_HPP
