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
struct fwrite_event : buffer
{
	using signature = size_t(char*, size_t, size_t, void*);
	fwrite_event(char* d, size_t s, size_t t, void*) noexcept
	: buffer{d, s*t} {};

	static void* dataptr(char*, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

struct easy_ref::header : fwrite_event
{
	using fwrite_event::fwrite_event;
};

struct easy_ref::read : fwrite_event
{
	using fwrite_event::fwrite_event;
};

struct easy_ref::write : fwrite_event
{
	using fwrite_event::fwrite_event;
	static constexpr size_t pause = CURL_WRITEFUNC_PAUSE;
};

struct easy_ref::debug : buffer
{
	using signature = int(CURL*, infotype, char*, size_t, void*);

	debug(CURL* e, infotype i, char* c, size_t s, void*) noexcept
	: buffer{c, s} , handle(e) , type(i) {}

	static void* dataptr(CURL*, infotype, char*, size_t, void* x) noexcept {
		return x;
	}

	easy_ref handle;
	infotype type;
};

struct easy_ref::seek
{
	using signature = int(void*, off_t, int);

	seek(void*, off_t offset, int origin) noexcept
	: offset(offset), origin(origin) {}

	static void* dataptr(void* x, off_t, int) noexcept
	{
		return x;
	}

	off_t offset;
	int origin;
};

struct easy_ref::progress
{
	using signature = int(void*, off_t, off_t, off_t, off_t);

	progress(void*, off_t dt, off_t dn, off_t ut, off_t un) noexcept
	: dltotal(dt), dlnow(dn), ultotal(ut), ulnow(un) {}

	static void* dataptr(void* x, off_t, off_t, off_t, off_t) noexcept
	{
		return x;
	}

	off_t dltotal, dlnow, ultotal, ulnow;
};
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
