#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "curl++/buffer.hpp"
#include "curl++/easy_info.hpp"
#include "curl++/easy_opt.hpp"
#include "curl++/invoke.hpp"
#include "curl++/option.hpp" // for handler
#include "curl++/types.hpp"  // for code
#include <cstddef>           // for size_t
#include <curl/curl.h>       // for CURL
namespace curl { // easy_ref

struct easy_ref
{
protected:
	friend struct multi_ref;
	CURL* handle = nullptr;
public:
	easy_ref() noexcept = default;
	easy_ref(CURL *handle) noexcept;

	/// @return handle != nullptr
	explicit operator bool() const noexcept;

	/** See curl_easy_pause.
	 * @throws curl::code
	 * @pre *this
	 */
	void pause(curl::pause flag);

	/** curl_easy_perform.
	 * @throws curl::code
	 * @pre *this
	 */
	void perform();

	/** curl_easy_setopt.
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code easy.setopt(o::url{"www.example.com"}); @endcode
	 */
	template<CURLoption o, typename T>
	void set(detail::easy_option<o, T> x)
	{
		invoke(curl_easy_setopt, handle, o, x.value);
	}

	/** curl_easy_getinfo
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code auto url = easy.getinfo(i::url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	inline auto get(info::info<i, T> x) const -> T
	{
		typename info::info<i, T>::value_type y;
		invoke(curl_easy_getinfo, handle, i, &y);
		return x(y);
	}

	// Events that can emitted by CURL
	struct debug;
	struct header;
	struct read;
	struct seek;
	struct write;
	struct progress;

	/** Set callback and dataptr from member functions of T.
	 * @pre *this
	 * @requires x->handle(E);
	 *
	 * Sets the *FUNCTION and *DATA options in one go.
	 * if !S emit a compiler error if x->handle(e) is invalid.
	 *
	 * example: @code x.set_handler<write>(foo); @endcode
	 */
	template<typename Event, bool NoError = false, typename T>
	inline void set_handler(T *x) noexcept
	{
		constexpr auto fptr = option::handler<Event>::template from_mem_fn<T>();
		static_assert(NoError || fptr, "Could not find `x->handle(e)`");
		fptr.easy(handle, x);
	}

	/** Set callback from static member function, and dataptr to nullptr.
	 * @pre *this
	 *
	 * usage: set_handler<write, T>();
	 */
	template<typename Event, typename T>
	inline void set_handler() noexcept
	{
		constexpr auto fptr = option::handler<Event>::template from_static_fn<T>();
		static_assert(fptr, "Could not find `T::handle(e)`");
		fptr.easy(handle, nullptr);
	}

	/** Set callback from static member function, and dataptr to param.
	 * @param T Contains static member functions for event.
	 * @param D Data pointer for event
	 * @pre *this
	 *
	 * usage: set_handler<write, T>(d*);
	 */
	template<typename Event, typename T, typename D>
	inline void set_handler(D *x) noexcept
	{
		constexpr auto fptr = option::handler<Event>::template from_static_fn<T, D>();
		static_assert(fptr, "Could not find `T::handle(e, d)`");
		fptr.easy(handle, x);
	}
};

} // namespace curl
namespace curl { // easy_ref::events

// TODO move event_info stuff into event types themselves.
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
namespace curl { // easy_handle

/** Lightweight RAII class for a curl easy handle. */
struct easy_handle : public easy_ref
{
	/** curl_easy_init.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();
	~easy_handle() noexcept;

	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

private:
	using easy_ref::handle;
};

} // namespace curl
namespace curl { // easy

/** CRTP easy handle that automatically sets callbacks and data ptrs.
 * @param T The parent type.
 */
template<typename T>
struct easy : public easy_handle
{
	easy() noexcept
	{
		set_handler< debug,    true >(self());
		set_handler< header,   true >(self());
		set_handler< read,     true >(self());
		set_handler< seek,     true >(self());
		set_handler< write,    true >(self());
		set_handler< progress, true >(self());
	}
private:
	auto self() noexcept -> T*
	{
		return static_cast<T*>(this);
	}
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_EASY_HPP
