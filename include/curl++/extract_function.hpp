#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>

namespace curl {
namespace detail {

/**
 * Used to convert parameter into reference or pointer depending on
 * whats needed.
 */
template<typename T>
struct to_ref_or_ptr_t {
	constexpr to_ref_or_ptr_t(T *value) noexcept :_value(value) {}
	constexpr operator T*() { return _value; };
	constexpr operator T&() { return *_value; }

	T *_value;
};

template<typename T>
auto to_ref_or_ptr(T* x) -> to_ref_or_ptr_t<T>
{
	return x;
}

/**
 * Extract event type from object
 */
template<typename F>
struct fptr_event;

template<typename R, typename H>
struct fptr_event<R(*)(H)>
{
	using type = std::decay_t<H>;
};

template<typename R, typename H, typename D>
struct fptr_event<R(*)(H, D)>
{
	using type = std::decay_t<H>;
};

template<typename F>
using fptr_event_t = typename fptr_event<F>::type;

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
		return state->on(event);
	}

	template<typename T>
	static auto invoke_static_fn(Args... args) -> R
	{
		auto event = E(args...);
		return T::on(event);
	}

	template<typename T, typename D>
	static auto invoke_static_fn_with_data(Args... args) -> R
	{
		auto event = E(args...);
		auto state = static_cast<D*>(E::dataptr(args...));
		return T::on(event, to_ref_or_ptr(state));
	}
#if __cplusplus >= 201703L
	template<auto F>
	static auto invoke_fptr(Args... args) -> R
	{
		auto event = E(args...);
		return F(event);
	}

	template<auto F, typename D>
	static auto invoke_fptr_with_data(Args... args) -> R
	{
		auto event = E(args...);
		auto state = static_cast<D*>(E::dataptr(args...));
		return F(event, to_ref_or_ptr(state));
	}
#endif
};

/*
 * Detectors
 */

/// detects t.on(e);
template<typename E, typename T>
using detect_mem_fn = decltype(void(std::declval<T>().on(std::declval<E>())));

/// detects T::on(e);
template<typename E, typename T>
using detect_static_fn = decltype(void(T::on(std::declval<E>())));

/// detects T::on(e, &d) or T::on(e, d);
template<typename E, typename T, typename D>
using detect_static_fn_with_data = decltype(
	void( T::on( std::declval<E>()
	           , to_ref_or_ptr(std::declval<D*>()))));

#if __cplusplus >= 201703L
/// Detectes F(e, &d) or F(e, d)
template<auto F, typename E, typename D>
using detect_fptr_with_data = decltype(
	void( F( std::declval<E>()
	       , to_ref_or_ptr(std::declval<D*>()))));

#endif
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

#if __cplusplus >= 201703L
template<auto F, typename E>
struct extract_fptr
{
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handler<E>::template invoke_fptr<F>;
	}
};


template<auto F, typename E, typename D, typename = void>
struct extract_fptr_with_data : detail::extract_default<E> {};

template<auto F, typename E, typename D>
struct extract_fptr_with_data<F, E, D, detect_fptr_with_data<F, E, D>>
{
	static constexpr
	typename E::signature* fptr() noexcept
	{
		return &detail::invoke_handler<E>::
			template invoke_fptr_with_data<F, D>;
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
	 * if NoError == false, then emit a compiler error if the on
	 * function doesnt exist.
	 */
	template<typename Event, bool NoError = false, typename T>
	void set_handler(T* x) noexcept
	{
		using fptr_t = typename Event::signature*;
		constexpr fptr_t fptr = extract_mem_fn<Event, T>::fptr();
		static_assert(NoError || fptr, "T does not have member function `on(Event)`");
		self().setopt(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().setopt(Event::DATA, fptr ? x    : nullptr);
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
		static_assert(NoError || fptr, "T does not have static member function `on(Event)`");
		self().setopt(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().setopt(Event::DATA, nullptr);
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
		static_assert(fptr, "T does not have static member function `on(Event, D*)`");
		self().setopt(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().setopt(Event::DATA, fptr ? x    : nullptr);
	}

#if __cplusplus >= 201703L
	/**
	 * Set handler from function pointer.
	 *
	 * example: @code
	 * size_t on_write(curl::easy::write w) { ...; return w.size(); }
	 * h.set_handler<&on_write>();
	 * @endcode
	 */
	template<auto F, typename E = fptr_event_t<decltype(F)>>
	void set_handler() noexcept
	{
		using fptr_t = typename E::signature*;
		constexpr fptr_t fptr = extract_fptr<F, E>::fptr();
		self().setopt(E::FUNC, fptr ? fptr : get_default<E>::fptr());
		self().setopt(E::DATA, nullptr);
	}

	/**
	 * Set handler from function pointer with data.
	 *
	 * example: @code
	 * size_t on_write(curl::easy::write w, int x) { ...; return w.size(); }
	 * int foo;
	 * h.set_handler<&on_write>(&foo);
	 * @endcode
	 */
	template<auto F, typename E = fptr_event_t<decltype(F)>, typename D>
	void set_handler(D* x) noexcept
	{
		using fptr_t = typename E::signature*;
		constexpr fptr_t fptr = extract_fptr_with_data<F, E, D>::fptr();
		static_assert(fptr, "F can not be called via `F(Event, D)`");
		self().setopt(E::FUNC, fptr ? fptr : get_default<E>::fptr());
		self().setopt(E::DATA, fptr ? x    : nullptr);
	}

#endif
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
		self().setopt(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().setopt(Event::DATA, nullptr);
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
		self().setopt(Event::FUNC, fptr ? fptr : get_default<Event>::fptr());
		self().setopt(Event::DATA, fptr ? x    : nullptr);
	}
#endif
};

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

