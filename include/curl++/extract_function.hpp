#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
#include <utility>
namespace curl {
namespace detail {

/**
 * Provides static functions to handle events in a variety of methods.
 *
 * @param E the event being handled.
 * @param 2 The function signature for the event.
 */
template<typename E, typename = typename E::signature>
struct invoke_handler;

template<typename E, typename R, typename ...Args>
struct invoke_handler<E, R(Args...)> {
	template<typename T>
	static auto invoke_mem_fn(Args... args) -> R
	{
		auto event = E(args...);
		auto state = static_cast<T*>(E::dataptr(args...));
		return state->handle(event);
	}

	template<typename T>
	static auto invoke_static_fn(Args... args) -> R
	{
		auto event = E(args...);
		return T::handle(event);
	}

	template<typename T, typename D>
	static auto invoke_static_fn_with_data(Args... args) -> R
	{
		auto event = E(args...);
		auto state = static_cast<D*>(E::dataptr(args...));
		return T::handle(event, state);
	}
#if 0
	template<auto fptr>
	static auto invoke_fptr(Args... args) -> R
	{
		auto event = E(args...);
		return fptr(event);
	}

	template<auto fptr, typename D>
	static auto invoke_fptr_with_data(Args... args) -> R
	{
		auto event = E(args...);
		auto state = static_cast<D*>(E::dataptr(args...));
		return fptr(event, state);
	}
#endif
};

/// detects t.handle(e);
template<typename E, typename T>
using detect_mem_fn = decltype(std::declval<T>().handle(std::declval<E>()), void());

/// detects T::handle(e);
template<typename E, typename T>
using detect_static_fn = decltype(T::handle(std::declval<E>()), void());

/// detects T::handle(e, &d);
template<typename E, typename T, typename D>
using detect_static_fn_with_data = decltype(T::handle(std::declval<E>(), std::declval<D*>()), void());

// Default extractor returning nullptr
template<typename E>
struct extract_default {
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return nullptr;
	}
};

} // namespace detail

template<typename E, typename T, typename = void>
struct extract_mem_fn : detail::extract_default<E> {};

template<typename E, typename T, typename = void>
struct extract_static_fn : detail::extract_default<E> {};

template<typename E, typename T, typename D, typename = void>
struct extract_static_fn_with_data : detail::extract_default<E> {};

/*
 * Specializations
 */

template<typename E, typename T>
struct extract_mem_fn<E, T, detail::detect_mem_fn<E, T>> {
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handler<E>::template invoke_mem_fn<T>;
	}
};

template<typename E, typename T>
struct extract_static_fn<E, T, detail::detect_static_fn<E, T>> {
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handler<E>::template invoke_static_fn<T>;
	}
};

template<typename E, typename T, typename D>
struct extract_static_fn_with_data<E, T, D, detail::detect_static_fn_with_data<E, T, D>> {
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handler<E>::template invoke_static_fn_with_data<T, D>;
	}
};

#if 0
template<auto x>
struct extract_fptr;

template<typename R, typename E, R(*fp)(E)>
struct extract_fptr<fp>
{
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handle<E>::template invoke_fptr<fp>;
	}
};
template<auto x, typename D>
struct extract_fptr_with_data;

template<typename R, typename E, typename D, R(*fp)(E, D*)>
struct extract_fptr<fp, D>
{
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handle<E>::template invoke_fptr_with_data<fp, D>;
	}
};
#endif

} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

