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
	char* data; size_t size;
};

/* easy event types */
namespace event {
// TODO fancier types with convenient functions
// consider better 
struct cleanup {};
struct debug   { curl::easy_ref handle; infotype info; buffer_t data; };
struct header  { buffer_t data; };
struct read    { buffer_t data; };
struct seek    { off_t offset; int origin; };
struct write   { buffer_t data; };
struct progress{ off_t dltotal, dlnow, ultotal, ulnow; };
} // namespace event

//@{
/** Event signatures used to match agains */
namespace detail {
template<class E> struct event_info
{
	using return_t = size_t;
	template<typename T>
	using signature = size_t(char *, size_t, size_t, T*);
	template<typename T>
	static return_t invoke(char *d, size_t s, size_t t, T* x) noexcept
	{
		return x->handle(E{buffer_t{d, s*t}});
	}
};
template<> struct event_info<event::cleanup>
{
	using return_t = int;
	template<typename T>
	using signature = int(T*);
	template<typename T>
	static return_t invoke(T *x) noexcept {
		return x->handle(event::cleanup{});
	}
};
template<> struct event_info<event::debug>
{
	using return_t = int;
	template<typename T>
	using signature = int(CURL*, infotype, char*, size_t, T*);
	template<typename T>
	static return_t invoke(CURL* e, infotype i, char* c, size_t s, T* x) noexcept
	{
		return x->handle(event::debug{e, i, buffer_t{c, s}});
	}
};
template<> struct event_info<event::seek>
{
	using return_t = int;
	template<typename T>
	using signature = int(T*, off_t, int);
	template<typename T>
	static return_t invoke(T* x, off_t offset, int origin) noexcept
	{
		return x->handle(event::seek{offset, origin});
	}
};
template<> struct event_info<event::progress>
{
	using return_t = int;
	template<typename T>
	using signature = int(T*, off_t, off_t, off_t, off_t);
	template<typename T>
	static return_t invoke(T* x, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	{
		return x->handle(event::progress{dt, dn, ut, un});
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
	static constexpr std::nullptr_t fptr() noexcept { return nullptr; }
};

template<typename E, typename T>
struct mem_fn<E, T, decltype(std::declval<T>().handle(E{}), void())>
{
	static constexpr
	typename event_info<E>::template signature<T>* fptr() noexcept
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
	, data(x)
	{ /* NOOP */ }

	/// Construct data from D* argument, and function pointer from static
	/// member function of T
	template<typename T, typename D>
	callback(T*, D* x) noexcept
	: fptr(/* detail::bar<S, T>::fptr() */)
	, data(x)
	{ /* NOOP */ }

	// sets the callback on the raw handle
	void set_handler(CURL *) const noexcept;
private:
	// type erased function pointer taking void*
	typename event_info<event>::template signature<void>* fptr;
	// type erased data passed to
	void* data;
};

} // namespace detail
//@}
} // namespace curl

#endif // CURLPLUSPLUS_EVENT_HPP
