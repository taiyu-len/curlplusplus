#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "curl++/buffer.hpp"
#include "curl++/easy_info.hpp"
#include "curl++/extract_function.hpp"
#include "curl++/easy_opt.hpp"
#include "curl++/invoke.hpp"
#include "curl++/option.hpp" // for handler
#include "curl++/types.hpp"  // for code
#include <cstddef>           // for size_t
#include <utility>
#include <curl/curl.h>       // for CURL
namespace curl { // easy_ref

/** A lightweight non-owning wrapper around a curl easy handle type.
 *
 * All member functions unless specified otherwise are wrappers around
 * curl_easy_* functions.
 * If they fail they will throw a curl::code.
 * All require *this == true, except for init
 *
 **** reset(CURL*)
 * Cleans up existing handle and changes it.
 *
 **** operator bool()
 * returns true if handle is set.
 *
 **** set(...)
 * setopt wrapper taking easy_option templated type.
 **** get(...)
 * getinfo wrapper taking easy_info templated type.
 **** set_handler(...)
 * setopt wrapper for CURLOPT_???FUNCTION and CURLOPT_???DATA settings.
 * provides various ways to set the function and data.
 */
struct easy_ref {
protected:
	friend struct multi_ref;
	CURL* _handle = nullptr;
public:
	enum pause_flags : long {
		pause_recv = CURLPAUSE_RECV,
		pause_send = CURLPAUSE_SEND,
		pause_all  = CURLPAUSE_ALL,
		pause_cont = CURLPAUSE_CONT
	};

	// Events that can emitted by CURL
	struct debug;
	struct header;
	struct read;
	struct seek;
	struct write;
	struct progress;

	easy_ref() noexcept = default;
	easy_ref(CURL *h) noexcept : _handle(h) {};

	explicit operator bool() const noexcept { return _handle; }

	/// cleanup existing handle and replace it with a new handle.
	void init();
	/// cleanup existing handle and replace it with given handle.
	void reset(CURL* = nullptr) noexcept;

	void pause(pause_flags);
	void perform();

	template<CURLoption o, typename T>
	inline void set(option::detail::easy_option<o, T> x);

	template<CURLINFO i, typename T>
	inline auto get(info::info<i, T> x) const -> T;

	// TODO pull all this junk out to its own class because multi does all
	// the same stuff too.
	/** Set handler for event to member function of T.
	 * Sets the *FUNCTION and *DATA options in one go.
	 * if !S emit a compiler error if x->handle(e) is invalid.
	 *
	 * example: @code x.set_handler<write>(foo); @endcode
	 */
	template<typename Event, bool NoError = false, typename T>
	inline void set_handler(T *x) noexcept;

	/** Set handler for event to static function of T.
	 * example: @code set_handler<write, T>(); @endcode
	 */
	template<typename Event, typename T, bool NoError = false>
	inline void set_handler() noexcept;

	/** Set handler for event to static function of T with user specified data.
	 * example: @code set_handler<write, T>(&d); @endcode
	 */
	template<typename Event, typename T, typename D>
	inline void set_handler(D *x) noexcept;
#if 0
	/** Set callback from a lambda.
	 *
	 * only works in c++17 due to constexpr conversion to function pointer.
	 * and does not work in g++ due to a bug
	 *   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83258
	 *
	 * sfinae is used to check if +T gives a value and is constexpr.
	 *
	 */
	template<typename T,
		typename F = decltype(+std::declval<T>()),
		F x = +std::declval<T>()>
	inline void set_handle(T x) noexcept
	{
		constexpr auto fptr = +x;
		constexpr fptr_t fptr = extract_fptr<fptr>::fptr();
		Event::setopt(_handle, fptr, nullptr);
	}

	/** Set callback from a lambda and data pointer.
	 * same as above.
	 */
	template<typename T, typename D,
		typename F = decltype(+std::declval<T>()),
		F x = +std::declval<T>()>
	inline void set_handle(T x, D* y) noexcept
	{
		constexpr auto fptr = +x;
		constexpr fptr_t fptr = extract_fptr_with_data<fptr, D>::fptr();
		Event::setopt(_handle, fptr, y);
	}
#endif
};

template<CURLoption o, typename T>
void easy_ref::set(option::detail::easy_option<o, T> x)
{
	invoke(curl_easy_setopt, _handle, o, x.value);
}

template<CURLINFO i, typename T>
auto easy_ref::get(info::info<i, T> x) const -> T
{
	typename info::info<i, T>::value_type y;
	invoke(curl_easy_getinfo, _handle, i, &y);
	return x(y);
}

template<typename Event, bool NoError, typename T>
void easy_ref::set_handler(T *x) noexcept
{
	// required to avoid a buggy gcc warning
	using fptr_t = typename Event::signature*;
	constexpr fptr_t fptr = extract_mem_fn<Event, T>::fptr();
	static_assert(NoError || fptr, "T does not have member function `handle(e)`");
	Event::setopt(_handle, fptr, x);
}

template<typename Event, typename T, bool NoError>
void easy_ref::set_handler() noexcept
{
	using fptr_t = typename Event::signature*;
	constexpr fptr_t fptr = extract_static_fn<Event, T>::fptr();
	static_assert(NoError || fptr, "Could not find `T::handle(e)`");
	Event::setopt(_handle, fptr, nullptr);
}

template<typename Event, typename T, typename D>
void easy_ref::set_handler(D *x) noexcept
{
	using fptr_t = typename Event::signature*;
	constexpr fptr_t fptr = extract_static_fn_with_data<Event, T, D>::fptr();
	static_assert(fptr, "Could not find `T::handle(e, d)`");
	Event::setopt(_handle, fptr, x);
}

} // namespace curl
namespace curl { // easy_ref::events

template<typename T>
struct fevent_base : T {
	using signature = size_t(char*, size_t, size_t, void*);

	fevent_base(char* d, size_t s, size_t t, void*) noexcept
	: T{d, s*t} {};

	static auto dataptr(char*, size_t, size_t, void* x) noexcept -> void*
	{
		return x;
	}
};
using fwrite_event = fevent_base<const_buffer>;
using fread_event  = fevent_base<buffer>;

struct easy_ref::header : fwrite_event {
	using fwrite_event::fwrite_event;

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::read : fread_event {
	using fread_event::fread_event;

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::write : fwrite_event {
	using fwrite_event::fwrite_event;

	static constexpr size_t pause = CURL_WRITEFUNC_PAUSE;

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::debug : const_buffer {
	using signature = int(CURL*, infotype, char*, size_t, void*);

	easy_ref handle;
	infotype type;

	debug(CURL* e, infotype i, char* c, size_t s, void*) noexcept
	: const_buffer{c, s} , handle(e) , type(i) {}

	static auto dataptr(CURL*, infotype, char*, size_t, void* x) noexcept -> void*
	{
		return x;
	}

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::seek {
	using signature = int(void*, curl_off_t, int);

	curl_off_t offset;
	int origin;

	seek(void*, curl_off_t offset, int origin) noexcept
	: offset(offset), origin(origin) {}

	static auto dataptr(void* x, curl_off_t, int) noexcept -> void*
	{
		return x;
	}

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::progress {
	using signature = int(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

	curl_off_t dltotal, dlnow, ultotal, ulnow;

	progress(void*, curl_off_t dt, curl_off_t dn,
	         curl_off_t ut, curl_off_t un) noexcept
	: dltotal(dt), dlnow(dn), ultotal(ut), ulnow(un) {}

	static auto dataptr(void* x, curl_off_t, curl_off_t,
	                    curl_off_t, curl_off_t) noexcept -> void*
	{
		return x;
	}

	static void setopt(CURL*, signature*, void*) noexcept;
};
} // namespace curl
namespace curl { // easy

/** Lightweight RAII wrapper for a curl easy handle. */
struct easy : public easy_ref {
	easy();
	explicit easy(easy_ref er) noexcept : easy_ref(er) {};

	~easy() noexcept;

	easy(easy &&) noexcept;
	easy& operator=(easy &&) noexcept;

	auto release() -> easy_ref { return std::exchange(_handle, nullptr); }
private:
	// hide raw handle from inheriting types.
	using easy_ref::_handle;
};

} // namespace curl
namespace curl { // easy_base

/** CRTP base type that automatically sets callbacks and data ptrs.
 * @param T The parent type.
 */
template<typename T>
struct easy_base : public easy {
	easy_base() noexcept
	{
		set_handler< debug,    true >(self());
		set_handler< header,   true >(self());
		set_handler< read,     true >(self());
		set_handler< seek,     true >(self());
		set_handler< write,    true >(self());
		set_handler< progress, true >(self());
	}
private:
	auto self() noexcept -> T* { return static_cast<T*>(this); }
	// prevent slicing
	operator easy() = delete;

	using easy::easy;
	using easy::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_EASY_HPP
