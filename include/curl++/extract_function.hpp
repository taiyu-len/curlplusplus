#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include "callback_wrapper.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>

namespace curl {
using userptr = callback_wrapper::userptr;
namespace detail {
/*
 * Extract function pointer for default callback if it exists.
 */
template<typename T, typename = void>
struct get_default {
	static constexpr auto fptr() noexcept -> callback_wrapper::signature<T>*
	{
		return nullptr;
	}
};

template<typename T>
struct get_default<T, decltype(void(&T::DEFAULT))> {
	static constexpr auto fptr() noexcept -> callback_wrapper::signature<T>*
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
private:
	auto self() -> Parent&
	{
		return *static_cast<Parent*>(this);
	}

	template<typename E, typename T>
	void setoptions(T x, void* y)
	{
		self().setopt(E::FUNC, x ? x : get_default<E>::fptr());
		self().setopt(E::DATA, x ? y : nullptr);
	}
public:
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
		constexpr callback_wrapper::signature<Event>* f =
		          callback_wrapper::wrap_member_fn<Event, T>::value;
		static_assert(NoError || f, "T does not have member function `on(Event)`");
		setoptions<Event>(f, x);
	}

	/**
	 * Set handler for event to static member function of T.
	 *
	 * example: @code set_handler<write, T>(); @endcode
	 */
	template<typename Event, typename T, bool NoError = false>
	void set_handler() noexcept
	{
		constexpr callback_wrapper::signature<Event>* f =
		          callback_wrapper::wrap_static_fn<Event, T>::value;
		static_assert(NoError || f, "T does not have static member function `on(Event)`");
		setoptions<Event>(f, nullptr);
	}

	/**
	 * Set handler for event to static function of T with user specified data.
	 *
	 * example: @code set_handler<write, T>(&d); @endcode
	 */
	template<typename Event, typename T, typename D>
	void set_handler(D *x) noexcept
	{
		constexpr callback_wrapper::signature<Event>* f =
		          callback_wrapper::wrap_static_userptr_fn<Event, T, D>::value;
		static_assert(f, "T does not have static member function `on(Event, D*)`");
		setoptions<Event>(f, x);
	}

	template<typename T, T fptr, typename E = callback_wrapper::fptr_event<T>>
	void set_handler() noexcept
	{
		constexpr callback_wrapper::signature<E>* f =
		          callback_wrapper::wrap_function<E, T, fptr>::value;
		setoptions<E>(f, nullptr);
	}

	template<typename T, T fptr, typename D, typename E = callback_wrapper::fptr_event<T>>
	void set_handler(D *x) noexcept
	{
		constexpr callback_wrapper::signature<E>* f =
		          callback_wrapper::wrap_function_userptr<E, T, fptr, D>::value;
		setoptions<E>(f, x);
	}
	
#if __cplusplus >= 201703L
	template<auto F>
	void set_handler() noexcept
	{
		set_handler<decltype(F), F>();
	}

	template<auto F, typename D>
	void set_handler(D* x) noexcept
	{
		set_handler<decltype(F), F, D>(x);
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
	template< typename T
	        , typename F = decltype(+std::declval<T>())
	        , typename E = callback_wrapper::fptr_event<F>
	        , F = +std::declval<T>()>
	void set_handle(T x) noexcept
	{
		constexpr callback_wrapper::signature<E>* f =
		          callback_wrapper::wrap_function<E, T, +x>::value;
		setoptions<E>(f, nullptr);
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
		constexpr callback_wrapper::signature<E>* f =
		          callback_wrapper::wrap_function<E, T, +x>::value;
		setoptions<E>(f, y);
	}
#endif
};

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

