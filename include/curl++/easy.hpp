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

struct easy_ref {
protected:
	friend struct multi_ref;
	CURL* handle = nullptr;
public:
	easy_ref() noexcept = default;
	easy_ref(CURL *h) noexcept : handle(h) {};

	/// @return handle != nullptr
	explicit operator bool() const noexcept { return handle; }

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
	 * example: @code easy.set(o::url{"www.example.com"}); @endcode
	 */
	template<CURLoption o, typename T>
	void set(option::detail::easy_option<o, T> x)
	{
		invoke(curl_easy_setopt, handle, o, x.value);
	}

	/** curl_easy_getinfo
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code auto url = easy.getinfo(i::url); @endcode
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
		// required to avoid a buggy gcc warning
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_mem_fn<Event, T>::fptr();
		static_assert(NoError || fptr, "Could not find `x->handle(e)`");
		Event::setopt(handle, fptr, x);
	}

	/** Set callback from static member function, and dataptr to nullptr.
	 * @pre *this
	 *
	 * usage: set_handler<write, T>();
	 */
	template<typename Event, typename T>
	inline void set_handler() noexcept
	{
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_static_fn<Event, T>::fptr();
		static_assert(fptr, "Could not find `T::handle(e)`");
		Event::setopt(handle, fptr, nullptr);
	}

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
		Event::setopt(handle, fptr, nullptr);
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
		Event::setopt(handle, fptr, y);
	}
#endif

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
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_static_fn_with_data<Event, T, D>::fptr();
		static_assert(fptr, "Could not find `T::handle(e, d)`");
		Event::setopt(handle, fptr, x);
	}
};

} // namespace curl
namespace curl { // easy_ref::events
namespace easy_events {
using debug    = easy_ref::debug;
using header   = easy_ref::header;
using read     = easy_ref::read;
using seek     = easy_ref::seek;
using write    = easy_ref::write;
using progress = easy_ref::progress;
}

template<typename T>
struct fwrite_event : T {
	using signature = size_t(char*, size_t, size_t, void*);

	fwrite_event(char* d, size_t s, size_t t, void*) noexcept
	: T{d, s*t} {};

	static void* dataptr(char*, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

struct easy_ref::header : fwrite_event<const_buffer> {
	using fwrite_event::fwrite_event;

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::read : fwrite_event<buffer> {
	using fwrite_event::fwrite_event;

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::write : fwrite_event<const_buffer> {
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

	static void* dataptr(CURL*, infotype, char*, size_t, void* x) noexcept
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

	static void* dataptr(void* x, curl_off_t, int) noexcept
	{
		return x;
	}

	static void setopt(CURL*, signature*, void*) noexcept;
};

struct easy_ref::progress {
	using signature = int(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

	curl_off_t dltotal, dlnow, ultotal, ulnow;

	progress(void*, curl_off_t dt, curl_off_t dn, curl_off_t ut, curl_off_t un) noexcept
	: dltotal(dt), dlnow(dn), ultotal(ut), ulnow(un) {}

	static void* dataptr(void* x, curl_off_t, curl_off_t, curl_off_t, curl_off_t) noexcept
	{
		return x;
	}

	static void setopt(CURL*, signature*, void*) noexcept;
};
} // namespace curl
namespace curl { // easy_handle

/** Lightweight RAII class for a curl easy handle. */
struct easy_handle : public easy_ref {
	/** curl_easy_init.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();

	/** Take ownership of an easy_ref.
	 * @warning watch out for multiple owners.
	 */
	explicit easy_handle(easy_ref er) noexcept : easy_ref(er) {};
	~easy_handle() noexcept;

	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

	// release ownership of handle
	auto release() -> easy_ref { return std::exchange(handle, nullptr); }

private:
	using easy_ref::handle;
};

} // namespace curl
namespace curl { // easy

/** CRTP easy handle that automatically sets callbacks and data ptrs.
 * @param T The parent type.
 */
template<typename T>
struct easy : public easy_handle {
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
	auto self() noexcept -> T* { return static_cast<T*>(this); }
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_EASY_HPP
