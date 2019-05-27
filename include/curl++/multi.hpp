#ifndef CURLPLUSPLUS_MULTI_HPP
#define CURLPLUSPLUS_MULTI_HPP
#include "curl++/easy.hpp"        // for easy_ref
#include "curl++/invoke.hpp"
#include "curl++/multi_opt.hpp"
#include "curl++/option.hpp"      // for handler
#include "curl++/types.hpp"       // for mcode
#include <chrono>                 // for milliseconds
#include <curl/curl.h>
#include <iterator>
namespace curl { // multi_ref

// TODO lots of work refining the usage of this class.
// as it is now its a very light wrapper over the c-api and exposes raw bits
// here and there.

struct multi_ref {
protected:
	CURLM* _handle = nullptr;
public:
	// an iterable object that produces info_read_messages
	struct info_read_proxy;
	struct info_read_message;

	// events
	struct push;
	struct socket;
	struct timer;

	multi_ref() noexcept = default;
	multi_ref(CURLM* h) noexcept : _handle(h) {}

	explicit operator bool() const noexcept { return _handle; }

	void init();
	void reset(CURLM* = nullptr) noexcept;

	auto info_read() noexcept -> info_read_proxy;
	auto perform() -> int;
	auto socket_action(socket_t, int ev_bitmask) -> int;
	auto wait(std::chrono::milliseconds) -> int;
	void add_handle(easy_ref);
	void assign(socket_t, void* data);
	void remove_handle(easy_ref);

	template<CURLMoption o, typename T>
	inline void set(option::detail::multi_option<o, T> x)
	{
		invoke(curl_multi_setopt, _handle, o, x.value);
	}

	/** wrapper for curl_multi_setopt for functions and data from an object.
	 * @pre *this
	 *
	 * Sets the *FUNCTION and *DATA options in one go.
	 * if !S emit a compiler error if x->handle(e) is invalid.
	 *
	 * example: @code x.set_handler<write>(foo); @endcode
	 */
	template<typename Event, bool NoError = false, typename T>
	inline void set_handler(T *x) noexcept
	{
		constexpr auto fptr = extract_mem_fn<Event, T>::fptr();
		static_assert(NoError || fptr, "T does not have member function handle(Event)");
		Event::setopt(_handle, fptr, x);
	}

	/** wrapper for curl_multi_setopt for function from static member
	 * functions.
	 * @pre *this
	 *
	 * example: @code set_handler<write, T>(); @endcode
	 */
	template<typename Event, typename T>
	inline void set_handler() noexcept
	{
		constexpr auto fptr = extract_static_fn<Event, T>::fptr();
		static_assert(fptr, "T does not have static member function handle(Event)");
		Event::setopt(_handle, fptr, nullptr);
	}

	/** wrapper for curl_multi_setopt for functions and data from static
	 * member function and data pointer.
	 *
	 * @param T Contains static member functions for event.
	 * @param D Data pointer for event
	 * @pre *this
	 *
	 * example: @code set_handler<write, T>(d*); @endcode
	 */
	template<typename Event, typename T, typename D>
	inline void set_handler(D *x) noexcept
	{
		constexpr auto fptr = extract_static_fn_with_data<Event, T, D>::fptr();
		static_assert(fptr, "T does not have static member function handle(Event, D*)");
		Event::setopt(_handle, fptr, x);
	}
};

} // namespace curl
namespace curl { // multi_ref::info_read_proxy

struct multi_ref::info_read_message {
	CURLMSG  msg;
	easy_ref ref;
	code     result;
	void*    whatever;
	auto operator->() -> info_read_message* { return this; }
};

struct multi_ref::info_read_proxy {
	struct iterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = CURLMsg;
		using difference_type = int;
		using reference = multi_ref::info_read_message;
		using pointer   = multi_ref::info_read_message;

		iterator() noexcept = default;
		iterator(CURLM *) noexcept;

		bool operator!=(iterator x) noexcept
		{
			return _message != x._message;
		}
		auto operator++() noexcept -> iterator&;
		auto operator*()  noexcept -> reference;
		auto operator->() noexcept -> pointer  { return **this; }
		auto remaining() const noexcept -> int { return _remaining; }

	private:
		CURLM*   _handle    = nullptr;
		CURLMsg* _message   = nullptr;
		int      _remaining = 0;
	};
	info_read_proxy(CURLM * h): _handle(h) {}
	auto begin() -> iterator { return {_handle}; }
	auto end()   -> iterator { return {}; }

private:
	CURLM*   _handle;
};

} // namespace curl
namespace curl { // multi_ref::events

// TODO implement
struct multi_ref::push {
	using signature = int(CURL*, CURL*, size_t, curl_pushheaders*, void*);

	push(CURL*, CURL*, size_t, curl_pushheaders*, void*);

	static void* dataptr(CURL*, CURL*, size_t, curl_pushheaders*, void* x) noexcept
	{
		return x;
	}

	static void setopt(CURLM*, signature*, void*) noexcept;
};

struct multi_ref::socket {
	using signature = int(CURL*, curl_socket_t, int, void*, void*);
	enum poll
	{
		in     = CURL_POLL_IN,
		out    = CURL_POLL_OUT,
		inout  = CURL_POLL_INOUT,
		remove = CURL_POLL_REMOVE
	};

	easy_ref      easy;
	curl_socket_t sock;
	poll          what;
	void*         data;

	socket(CURL* e, curl_socket_t s, int w, void*, void* d) noexcept
	: easy(e), sock(s), what(static_cast<poll>(w)), data(d) {}

	static void* dataptr(CURL*, curl_socket_t, int, void* x, void*) noexcept
	{
		return x;
	}

	static void setopt(CURLM*, signature*, void*) noexcept;
};

struct multi_ref::timer {
	using signature = int(CURLM*, long, void*);
	using milliseconds = std::chrono::milliseconds;

	multi_ref    multi;
	milliseconds timeout;

	timer(CURLM* m, long timeout_ms, void*) noexcept
	: multi(m), timeout(timeout_ms) {}

	static void* dataptr(CURLM*, long, void* x) noexcept
	{
		return x;
	}

	static void setopt(CURLM*, signature*, void*) noexcept;
};

} // namespace curl
namespace curl { // mutli

struct multi : public multi_ref {
	multi();
	~multi() noexcept;

	multi(multi &&) noexcept;
	multi& operator=(multi &&) noexcept;
private:
	using multi_ref::_handle;
};

} // namespace curl
namespace curl { // multi

/** CRTP multi handle that automatically sets callbacks and data.
 * @param T the parent type.
 */
template<typename T>
struct multi_base : public multi {
	multi_base() noexcept
	{
		// set event handlers conditionally
		set_handler< push,   true >(self());
		set_handler< timer,  true >(self());
		set_handler< socket, true >(self());
	}

private:
	auto self() noexcept -> T* { return static_cast<T*>(this); }
	using multi::multi;
	using multi::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_MULTI_HPP
