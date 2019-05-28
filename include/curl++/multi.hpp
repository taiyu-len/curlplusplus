#ifndef CURLPLUSPLUS_MULTI_HPP
#define CURLPLUSPLUS_MULTI_HPP
#include "curl++/easy.hpp"        // for easy_ref
#include "curl++/info_read.hpp"
#include "curl++/invoke.hpp"
#include "curl++/multi_opt.hpp"
#include "curl++/option.hpp"      // for handler
#include "curl++/types.hpp"       // for mcode
#include <chrono>                 // for milliseconds
#include <curl/curl.h>
#include <iterator>
#include <utility>

namespace curl {

// TODO lots of work refining the usage of this class.
// as it is now its a very light wrapper over the c-api and exposes raw bits
// here and there.

/**
 * Non-owning wrapper for a curl multi handle.
 */
struct multi_ref {
protected:
	CURLM* _handle = nullptr;
public:
	// events
	struct push;
	struct socket;
	struct timer;

	/**
	 * Default constuct empty handle.
	 */
	multi_ref() noexcept = default;

	/**
	 * Construct with given raw handle.
	 */
	multi_ref(CURLM* h) noexcept
	: _handle(h)
	{}

	/**
	 * @return true iff handle is valid.
	 */
	explicit operator bool() const noexcept
	{
		return _handle;
	}

	/**
	 * Cleanup existing handle and set to given raw handle.
	 */
	void reset(CURLM* new_handle = nullptr) noexcept
	{
		curl_multi_cleanup(std::exchange(_handle, new_handle));
	}

	/**
	 * Cleanup existing handle and set to a new handle.
	 *
	 * @throws std::runtime_error on failure to create handle.
	 * @warning if there is an existing handle it may have things that need
	 * to be cleaned up prior to this call.
	 */
	void init()
	{
		auto new_handle = curl_multi_init();
		if (new_handle == nullptr)
		{
			throw std::runtime_error("failed to initialize multi handle");
		}
		reset(new_handle);
	}
	/**
	 * see curl_multi_perform.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto perform() -> int
	{
		return invoke_r<int>(curl_multi_perform, _handle);
	}

	/**
	 * see curl_multi_add_handle
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void add_handle(easy_ref ref)
	{
		CURL* raw_easy_handle = ref._handle;
		invoke(curl_multi_add_handle, _handle, raw_easy_handle);
	}

	/**
	 * see curl_multi_remove_handle.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void remove_handle(easy_ref ref)
	{
		CURL* raw_easy_handle = ref._handle;
		invoke(curl_multi_remove_handle, _handle, raw_easy_handle);
	}

	/**
	 * Returns an iterable object that can be iterated over to get messages
	 * about attached easy handles.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto info_read() noexcept -> info_read_proxy
	{
		return info_read_proxy{_handle};
	}


	/**
	 * see curl_multi_assign
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void assign(socket_t sockfd, void* data)
	{
		invoke(curl_multi_assign, _handle, sockfd, data);
	}

	/**
	 * see curl_multi_socket_action.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto socket_action(socket_t sockfd, int ev_bitmask) -> int
	{
		return invoke_r<int>(curl_multi_socket_action, _handle,
		                     sockfd, ev_bitmask);
	}

	/**
	 * see curl_multi_wait.
	 * simple use case for no extra fds.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto wait(std::chrono::milliseconds ms) -> int
	{
		return invoke_r<int>(curl_multi_wait, _handle, nullptr, 0,
		                     ms.count());
	}

	/**
	 * see curl_multi_setopt
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<CURLMoption o, typename T>
	void set(option::detail::multi_option<o, T> x)
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
	void set_handler(T *x) noexcept
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
	void set_handler() noexcept
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
	void set_handler(D *x) noexcept
	{
		constexpr auto fptr = extract_static_fn_with_data<Event, T, D>::fptr();
		static_assert(fptr, "T does not have static member function handle(Event, D*)");
		Event::setopt(_handle, fptr, x);
	}
};

/**
 * Owning RAII wrapper for a curl multi handle
 */
struct multi : public multi_ref {
	/**
	 * Construct with a valid handle.
	 */
	multi()
	{
		init();
	}

	/**
	 * Cleanup existing handle
	 */
	~multi() noexcept
	{
		reset();
	}

	/**
	 * Transfer ownership
	 */
	multi(multi&& x) noexcept
	{
		reset(std::exchange(x._handle, nullptr));
	}

	/**
	 * Transfer ownership
	 */
	multi& operator=(multi &&x) noexcept
	{
		reset(std::exchange(x._handle, nullptr));
		return *this;
	}

private:
	using multi_ref::_handle;
};


/**
 * CRTP multi handle that automatically sets callbacks and data.
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
	auto self() noexcept -> T*
	{
		return static_cast<T*>(this);
	}

	using multi::multi;

protected:
	using multi::set_handler;
};

} // namespace curl

#include "curl++/detail/multi_events.hpp"
#endif // CURLPLUSPLUS_MULTI_HPP
