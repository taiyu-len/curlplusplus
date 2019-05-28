#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
#include <utility>
namespace curl {
namespace detail {

/*
 * Method Invoke functions for an event
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

/*
 * Detectors
 */

/// detects t.handle(e);
template<typename E, typename T>
using detect_mem_fn = decltype(std::declval<T>().handle(std::declval<E>()), void());

/// detects T::handle(e);
template<typename E, typename T>
using detect_static_fn = decltype(T::handle(std::declval<E>()), void());

/// detects T::handle(e, &d);
template<typename E, typename T, typename D>
using detect_static_fn_with_data = decltype(T::handle(std::declval<E>(), std::declval<D*>()), void());

/*
 * Default Extractor types returning nullptr
 */

template<typename E>
struct extract_default {
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return nullptr;
	}
};

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

/*
 * Extract function pointer for default callback if it exists.
 */
template<typename T, typename = void>
struct get_default {
	static constexpr auto fptr() noexcept -> typename T::signature*
	{
		return nullptr;
	}
};

template<typename T>
struct get_default<T, decltype(void(&T::DEFAULT))> {
	static constexpr auto fptr() noexcept -> typename T::signature*
	{
		return &T::DEFAULT;
	}
};

/*
 * CRTP class for making the set_handler functions available for easy_ref and
 * multi_ref
 */
template<typename Parent>
struct set_handler_base {
	auto self() -> Parent&
	{
		return *static_cast<Parent*>(this);
	}

	/**
	 * Set handler for event to member function of T.
	 * Safely sets both the FUNCTION and DATA options for the specified
	 * event in one go.
	 *
	 * if NoError == false, then emit a compiler error if the handle
	 * function doesnt exist.
	 */
	template<typename Event, bool NoError = false, typename T>
	void set_handler(T* x) noexcept
	{
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_mem_fn<Event, T>::fptr();
		static_assert(NoError || fptr, "T does not have member function `handle(Event)`");
		self().set(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().set(Event::DATA, fptr ? x    : nullptr);
	}

	/**
	 * Set handler for event to static member function of T.
	 *
	 * example: @code set_handler<write, T>(); @endcode
	 */
	template<typename Event, typename T, bool NoError = false>
	void set_handler() noexcept
	{
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_static_fn<Event, T>::fptr();
		static_assert(NoError || fptr, "T does not have static member function `handle(Event)`");
		self().set(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().set(Event::DATA, nullptr);
	}

	/**
	 * Set handler for event to static function of T with user specified data.
	 *
	 * example: @code set_handler<write, T>(&d); @endcode
	 */
	template<typename Event, typename T, typename D>
	void set_handler(D *x) noexcept
	{
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_static_fn_with_data<Event, T, D>::fptr();
		static_assert(fptr, "T does not have static member function `handle(Event, D*)`");
		self().set(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().set(Event::DATA, fptr ? x    : nullptr);
	}

#if 0
	/**
	 * Set callback from a lambda.
	 *
	 * only works in c++17 due to constexpr conversion to function pointer.
	 * and does not work in g++ due to a bug
	 *   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83258
	 *
	 * could probably work around this by delaying the conversion to
	 * function pointer until within the invoke_handler functions
	 *
	 */
	template<typename T,
		typename F = decltype(+std::declval<T>()),
		F x = +std::declval<T>()>
	void set_handle(T x) noexcept
	{
		constexpr auto fptr = +x;
		constexpr fptr_t fptr = extract_fptr<fptr>::fptr();
		self().set(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().set(Event::DATA, nullptr);
	}

	/**
	 * Set callback from a lambda and data pointer.
	 * same as above.
	 */
	template<typename T, typename D,
		typename F = decltype(+std::declval<T>()),
		F x = +std::declval<T>()>
	void set_handle(T x, D* y) noexcept
	{
		constexpr auto fptr = +x;
		constexpr fptr_t fptr = extract_fptr_with_data<fptr, D>::fptr();
		self().set(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().set(Event::DATA, fptr ? x    : nullptr);
	}
#endif
};

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

