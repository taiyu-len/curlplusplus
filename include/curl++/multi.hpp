#ifndef CURLPLUSPLUS_MULTI_HPP
#define CURLPLUSPLUS_MULTI_HPP
#include "easy.hpp"        // for easy_ref
#include "handle_base.hpp"
#include "info_read.hpp"
#include "invoke.hpp"
#include "option.hpp"      // for handler
#include "types.hpp"       // for mcode

#include <chrono>          // for milliseconds
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
struct multi_ref
	: detail::handle_base<CURLM*>
	, detail::set_handler_base<multi_ref>
{
	enum pipelining : unsigned long {
		nothing   = CURLPIPE_NOTHING,
		HTTP1     = CURLPIPE_HTTP1,
		multiplex = CURLPIPE_MULTIPLEX,
	};

	struct push;
	struct socket;
	struct timer;

	using detail::handle_base<CURLM*>::handle_base;

	/**
	 * Cleanup existing handle and set to given raw handle.
	 */
	void reset(CURLM* new_handle = nullptr) noexcept
	{
		::curl_multi_cleanup(std::exchange(_handle, new_handle));
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
		auto new_handle = ::curl_multi_init();
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
		return invoke_r<int>(::curl_multi_perform, _handle);
	}

	/**
	 * see curl_multi_add_handle
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void add_handle(easy_ref ref)
	{
		invoke(::curl_multi_add_handle, _handle, ref.raw());
	}

	/**
	 * see curl_multi_remove_handle.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void remove_handle(easy_ref ref)
	{
		invoke(::curl_multi_remove_handle, _handle, ref.raw());
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
		invoke(::curl_multi_assign, _handle, sockfd, data);
	}

	/**
	 * see curl_multi_socket_action.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto socket_action(socket_t sockfd, int ev_bitmask) -> int
	{
		return invoke_r<int>(::curl_multi_socket_action, _handle,
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
		return invoke_r<int>(::curl_multi_wait, _handle, nullptr, 0,
		                     ms.count());
	}

	/**
	 * See curl_multi_timeout.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	auto timeout() -> std::chrono::milliseconds
	{
		return std::chrono::milliseconds(
			invoke_r<long>(::curl_multi_timeout, _handle));
	}

	/**
	 * see curl_multi_setopt
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<typename T>
	void setopt(CURLMoption o, T x)
	{
		invoke(::curl_multi_setopt, _handle, o, x);
	}

	/**
	 * Macro to define a function that can be used to set that particular
	 * option.
	 */
#define SETOPT_FUNC(NAME, OPTION, TYPE) \
	void NAME(detail::option<TYPE> x) { setopt(CURLMOPT_ ## OPTION, x); }

	SETOPT_FUNC(max_host_connections , MAX_HOST_CONNECTIONS , long);
	SETOPT_FUNC(max_total_connections, MAX_TOTAL_CONNECTIONS, long);
	SETOPT_FUNC(max_connects         , MAXCONNECTS          , long);

	void setopt(pipelining x) {
		setopt(CURLMOPT_PIPELINING, static_cast<unsigned long>(x));
	}

#undef SETOPT_FUNC
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

	multi(easy const&) = delete;
	auto operator=(multi const&) -> multi& = delete;

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

#include "multi_events.hpp"
#endif // CURLPLUSPLUS_MULTI_HPP
