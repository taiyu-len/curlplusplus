#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
#include <utility>
namespace curl {
namespace detail { /* event_info */
/** Struct to specialize to obtain function pointer for event and type.
 *
 * example: @code
 * template<> struct event_info<EVENT> {
 *   using signature = int(void*, int);
 *   static EVENT get_event(void*, int a) noexcept { return EVENT{a}; }
 *   static void* get_dataptr(void* x, int) noexcept { return x; }
 * };
 * @endcode
 */
template<typename E>
struct event_info;

} // namespace detail

namespace detail { // invoke_handler

/** Provides static functions to handle events in a variety of methods.
 * @param E the event being handled.
 * @param 2 The event signature.
 */
template<typename E, typename = typename event_info<E>::signature>
struct invoke_handler;
template<typename E, typename R, typename ...Args>

struct invoke_handler<E, R(Args...)>
{
	template<typename T>
	static auto invoke_mem_fn(Args... args) -> R
	{
		using info = event_info<E>;
		auto state = static_cast<T*>(info::get_dataptr(args...));
		auto event = info::get_event(args...);
		return state->handle(event);
	}

	template<typename T>
	static auto invoke_static_fn(Args... args) -> R
	{
		using info = event_info<E>;
		auto event = info::get_event(args...);
		return T::handle(event);
	}

	template<typename T, typename D>
	static auto invoke_static_fn_with_data(Args... args) -> R
	{
		using info = event_info<E>;
		auto state = static_cast<D*>(info::get_dataptr(args...));
		auto event = info::get_event(args...);
		return T::handle(event, state);
	}
};

/*
 * Extractor defaults
 */


template<typename E>
struct extract_default
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept
	{
		return nullptr;
	}
};

template<typename E, typename T, typename = void>
struct extract_mem_fn : extract_default<E> {};

template<typename E, typename T, typename = void>
struct extract_static_fn : extract_default<E> {};

template<typename E, typename T, typename D, typename = void>
struct extract_static_fn_with_data : extract_default<E> {};

/*
 * Event Handler Detectors
 */

// detects t.handle(e);
template<typename E, typename T>
using detect_mem_fn = decltype(std::declval<T>().handle(std::declval<E>()), void());

// detects T::handle(e);
template<typename E, typename T>
using detect_static_fn = decltype(T::handle(std::declval<E>()), void());

// detects T::handle(e, &d);
template<typename E, typename T, typename D>
using detect_static_fn_with_data = decltype(T::handle(std::declval<E>(), std::declval<D*>()), void());

/*
 * Extractor specializations for detected handles
 */

template<typename E, typename T>
struct extract_mem_fn<E, T, detect_mem_fn<E, T>>
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept
	{
		return &invoke_handler<E>::template invoke_mem_fn<T>;
	}
};

template<typename E, typename T>
struct extract_static_fn<E, T, detect_static_fn<E, T>>
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept
	{
		return &invoke_handler<E>::template invoke_static_fn<T>;
	}
};

template<typename E, typename T, typename D>
struct extract_static_fn_with_data<E, T, D, detect_static_fn_with_data<E, T, D>>
{
	static constexpr
	typename event_info<E>::signature* fptr() noexcept
	{
		return &invoke_handler<E>::template invoke_static_fn_with_data<T, D>;
	}
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

