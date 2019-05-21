#ifndef CURLPLUSPLUS_MULTI_HPP
#define CURLPLUSPLUS_MULTI_HPP
#include "curl++/easy.hpp"        // for easy_ref
#include "curl++/invoke.hpp"
#include "curl++/multi_opt.hpp"
#include "curl++/option.hpp"      // for handler
#include "curl++/types.hpp"       // for mcode
#include <curl/curl.h>
namespace curl { // multi_ref

struct multi_ref
{
protected:
	CURLM* handle = nullptr;
public:
	multi_ref() noexcept = default;
	multi_ref(CURLM *) noexcept;

	explicit operator bool() const noexcept { return handle; }

	void add_handle(easy_ref);
	void assign(socket_t, void* data);
	void socket_action(socket_t, int ev_bitmask, int* left);

	struct message;

	/** Read next message from multi handle. */
	auto info_read() -> message;

	/** Set a curl multi option.
	 * @pre handle != nullptr
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
		constexpr auto fptr = extract_mem_fn<Event, T>::fptr();
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
		constexpr auto fptr = extract_static_fn<Event, T>::fptr();
		static_assert(fptr, "Could not find `T::handle(e)`");
		Event::setopt(handle, fptr, nullptr);
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
		constexpr auto fptr = extract_static_fn_with_data<Event, T, D>::fptr();
		static_assert(fptr, "Could not find `T::handle(e, d)`");
		Event::setopt(handle, fptr, x);
	}
};

struct multi_ref::message
{
	CURLMSG msg;
	easy_ref easy_handle;
	union {
		void* whatever;
		code result;
	} data;
};

} // namespace curl
namespace curl { // multi_ref::events
// TODO implement these

struct multi_ref::push
{
	using signature = int(CURL*, CURL*, size_t, curl_pushheaders*, void*);
	push(CURL*, CURL*, size_t, curl_pushheaders*, void*);

	static void* dataptr(CURL*, CURL*, size_t, curl_pushheaders*, void* x) noexcept
	{
		return x;
	}
};

struct multi_ref::socket
{
	using signature = int(CURL*, curl_socket_t, void*, void*);
	socket(CURL*, curl_socket_t, void*, void*);

	static void* dataptr(CURL*, curl_socket_t, void* x, void*)
	{
		return x;
	}
};

struct multi_ref::timer
{
	using signature = int(CURLM*, long, void*);
	timer(CURLM*, long, void*);

	static void* dataptr(CURLM*, long, void* x)
	{
		return x;
	}
};

} // namespace curl
namespace curl { // mutli_handle

struct multi_handle : public multi_ref
{
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
struct multi : public multi_handle
{
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
