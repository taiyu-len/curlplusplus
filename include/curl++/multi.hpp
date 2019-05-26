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
	CURLM* handle = nullptr;
public:
	multi_ref() noexcept = default;
	multi_ref(CURLM *) noexcept;

	explicit operator bool() const noexcept { return handle; }

	void add_handle(easy_ref);
	void remove_handle(easy_ref);
	void assign(socket_t, void* data);
	auto perform() -> int;

	/** wrapper for curl_multi_socket_action.
	 * @return number of running handles.
	 * @throws curl::mcode
	 */
	auto socket_action(socket_t, int ev_bitmask) -> int;

	struct info_read_proxy;

	/** wrapper for curl_multi_info_read.
	 * @returns an iterable object returning curl message objects
	 */
	auto info_read() -> info_read_proxy;

	/** wrapper for curl_multi_wait */
	auto wait(std::chrono::milliseconds) -> int;

	/** wrapper for curl_multi_setopt.
	 * @pre *this
	 * @throws curl::mcode
	 */
	template<CURLMoption o, typename T>
	inline void set(option::detail::multi_option<o, T> x)
	{
		invoke(curl_multi_setopt, handle, o, x.value);
	}

	// Multi events
	struct push;
	struct socket;
	struct timer;

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
		Event::setopt(handle, fptr, x);
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
		Event::setopt(handle, fptr, nullptr);
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
		Event::setopt(handle, fptr, x);
	}
};

} // namespace curl
namespace curl { // multi_ref::info_read_proxy
struct multi_ref::info_read_proxy {
	struct iterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = CURLMsg;
		using difference_type = int;
		struct message {
			CURLMSG  msg;
			easy_ref ref;
			code     result;
			void*    whatever;
			message *operator->() { return this; }
		};
		using reference = message;
		using pointer   = message;

		iterator() noexcept = default;
		iterator(CURLM *) noexcept;

		bool operator!=(iterator x) noexcept
		{
			return _message != x._message;
		}
		auto operator++() noexcept -> iterator&;
		auto operator*()  noexcept -> message;
		auto operator->() noexcept -> message { return **this; }
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
namespace curl { // mutli_handle

struct multi_handle : public multi_ref {
	/** curl_multi_init.
	 * @throws std::runtime_error
	 */
	multi_handle();
	~multi_handle() noexcept;

	multi_handle(multi_handle &&) noexcept;
	multi_handle& operator=(multi_handle &&) noexcept;
private:
	using multi_ref::handle;
};

} // namespace curl
namespace curl { // multi

/** CRTP multi handle that automatically sets callbacks and data.
 * @param T the parent type.
 */
template<typename T>
struct multi : public multi_handle {
	multi() noexcept
	{
		// set event handlers conditionally
		set_handler< push,   true >(self());
		set_handler< timer,  true >(self());
		set_handler< socket, true >(self());
	}
	/** Read all messages and call handle(message).
	 *
	 * calls handle(message{}) from T (or not)
	 */
	void info_read_all() noexcept;
private:
	auto self() noexcept -> T* { return static_cast<T*>(this); }
	using multi_handle::multi_handle;
	using multi_handle::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_MULTI_HPP
