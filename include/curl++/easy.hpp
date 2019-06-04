#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "easy_info.hpp"
#include "easy_opt.hpp"
#include "extract_function.hpp"
#include "handle_base.hpp"
#include "invoke.hpp"
#include "option.hpp" // for handler
#include "types.hpp"

#include <cstddef>           // for size_t
#include <curl/curl.h>       // for CURL, curl_easy_*, etc
#include <stdexcept>         // for exceptions
#include <utility>           // for exchange
namespace curl {

/**
 * A light weight non-owning handle for a curl easy request.
 */
struct easy_ref
	: detail::handle_base<CURL*>
	, detail::set_handler_base<easy_ref>
{
	enum pause_flags : long {
		pause_recv = CURLPAUSE_RECV,
		pause_send = CURLPAUSE_SEND,
		pause_all  = CURLPAUSE_ALL,
		pause_cont = CURLPAUSE_CONT
	};

	// events that can emitted by CURL
	struct debug;
	struct header;
	struct read;
	struct seek;
	struct write;
	struct progress;

	using detail::handle_base<CURL*>::handle_base;

	/**
	 * cleanup existing handle and set to given raw handle.
	 */
	void reset(CURL* new_handle = nullptr) noexcept
	{
		::curl_easy_cleanup(std::exchange(_handle, new_handle));
	}

	/**
	 * resets existing handle or creates a new handle.
	 *
	 * @throws std::runtime_error on failure to create handle.
	 */
	void init()
	{
		if (_handle != nullptr) {
			::curl_easy_reset(_handle);
		} else {
			_handle = ::curl_easy_init();
		}
		if (_handle == nullptr) {
			throw std::runtime_error("failed to initialize easy handle");
		}
	}

	/**
	 * see curl_easy_pause().
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void pause(pause_flags flags)
	{
		invoke(::curl_easy_pause, _handle, static_cast<long>(flags));
	}

	/**
	 * see curl_easy_perform().
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void perform()
	{
		invoke(::curl_easy_perform, _handle);
	}

	/**
	 * see curl_easy_setopt().
	 * Sets options using types defined in easy_opt.hpp
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<CURLoption o, typename T>
	void set(option::detail::easy_option<o, T> x)
	{
		invoke(::curl_easy_setopt, _handle, o, x.value);
	}

	/**
	 * see curl_easy_setopt.
	 * set option manually.
	 *
	 * @warning Not type safe. avoid this
	 * @throws curl::code
	 * @pre *this
	 */
	template<typename T>
	void set(CURLoption o, T x)
	{
		invoke(::curl_easy_setopt, _handle, o, x);
	}

	/**
	 * see curl_easy_getinfo.
	 * Gets information for this request from variables defined in
	 * easy_info.hpp
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<CURLINFO I, typename T>
	auto get(info::detail::info<I, T> x) const -> T
	{
		auto y = x.value();
		invoke(::curl_easy_getinfo, _handle, I, &y);
		return x.convert(y);
	}
};

/**
 * Lightweight RAII wrapper for a curl easy handle.
 */
struct easy : public easy_ref {
	/**
	 * Construct with valid handle.
	 *
	 * @throws std::runtime_error
	 */
	easy()
	{
		init();
	}

	/**
	 * Take ownership from an easy_ref.
	 *
	 * @warning Beware multiple owners.
	 */
	explicit easy(easy_ref er) noexcept
	: easy_ref(er)
	{};

	easy(easy const&) = delete;
	auto operator=(easy const&) -> easy& = delete;

	/**
	 * Transfer ownership from given handle to this one.
	 */
	easy(easy&& x) noexcept
	: easy_ref(std::exchange(x._handle, nullptr))
	{}

	/**
	 * Transfer ownership from given handle to this one.
	 * Cleans up existing handle.
	 */
	auto operator=(easy&& x) noexcept -> easy&
	{
		reset(std::exchange(x._handle, nullptr));
		return *this;
	}

	/**
	 * Cleanup handle.
	 */
	~easy() noexcept
	{
		reset();
	}

	/**
	 * Release owner ship of easy handle and return non-owning handle.
	 */
	auto release() noexcept -> easy_ref
	{
		return std::exchange(_handle, nullptr);
	}
private:
	// hide raw handle from inheriting types.
	using easy_ref::_handle;
};

/**
 * CRTP base type that automatically sets callbacks and data ptrs.
 *
 * @param T The parent type.
 */
template<typename T>
struct easy_base : public easy {
	/**
	 * Sets up handlers for events to use member functions from parent
	 * class, or do nothing if there is no handler.
	 */
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
	auto self() noexcept -> T*
	{
		return static_cast<T*>(this);
	}

	using easy::easy;

protected:
	using easy::set_handler;
};

} // namespace curl
#include "curl++/detail/easy_events.hpp"
#endif // CURLPLUSPLUS_EASY_HPP
