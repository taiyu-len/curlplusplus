#ifndef CALLBACK_WRAPPER_HPP
#define CALLBACK_WRAPPER_HPP
/**
 * Takes functions of various forms and converts it into a typesafe
 * function for C style callbacks.
 *
 * example usage:
 * @code
 *   struct Event {
 *     using signature = void(int, int, callback_wrapper::userptr);
 *     Event(int, int, void*);
 *   };
 *
 *   struct Callback {
 *     on(Event) {
 *       // do things with event
 *     }
 *   };
 *
 *  void some_function() {
 *     auto cb = Callback(some_state);
 *     auto fp = callback_wrapper::wrap_member_fn<Event, Callback>::value;
 *
 *     some_api.set_callback(fp);
 *   }
 * @endcode
 * Takes functions of the form R(Event) and converts it to R(a,b,c) for use in
 * c apis.
 */
#include <type_traits>
namespace callback_wrapper {
/**
 * Psuedo Type To declare an argument in the C signature of an event as the user
 * set pointer.
 */
struct userptr {
	constexpr userptr(void* v) noexcept : _value(v) {};
	constexpr operator void*() const noexcept { return _value; }
private:
	void* _value;
};

namespace detail { // userp_to_voidp

/**
 * Metafunction to transform userptr to void*
 */
template<typename T>
using userp_to_voidp = typename std::conditional_t
	< std::is_same<T, userptr>::value
	, void*
	, T>;

} // namespace detail
namespace detail { // signature_t

template<typename>
struct signature_t;

template<typename R, typename... Args>
struct signature_t<R(Args...)>
{
	using type = R(detail::userp_to_voidp<Args>...);
};

} // namespace detail
namespace detail { // fptr_event_t

template<typename>
struct fptr_event_t;

template<typename R, typename E, typename... Args>
struct fptr_event_t<R(*)(E, Args...)>
{
	using type = std::decay_t<E>;
};


} // namespace detail

template<typename E>
using signature = typename detail::signature_t<typename E::signature>::type;

template<typename T>
using fptr_event = typename detail::fptr_event_t<T>::type;

namespace detail { // get_userptr

/**
 * Gets the first userptr param from the arguments.
 */

template<typename... Args>
constexpr void* get_userptr_impl(userptr p, Args const&...) noexcept
{
	return p;
}

template<typename T, typename... Args>
constexpr void* get_userptr_impl(T const&, Args const&... args) noexcept
{
	return get_userptr_impl(args...);
}

template<typename... Args>
constexpr void* get_userptr(Args const&... args) noexcept
{
	return get_userptr_impl(args...);
}

} // namespace detail
namespace detail { // to_ref_or_ptr

/**
 * Used to convert parameter into reference or pointer depending on
 * whats needed.
 */
template<typename T>
struct to_ref_or_ptr_t {
	constexpr to_ref_or_ptr_t(T *value) noexcept :_value(value) {}
	constexpr operator T*() { return  _value; };
	constexpr operator T&() { return *_value; }
private:
	T *_value;
};

template<typename T>
auto to_ref_or_ptr(T* x) -> to_ref_or_ptr_t<T>
{
	return x;
}

} // namespace detail
namespace detail { // callback_wrappers

/**
 * Contains the various kinds of callbacks functions that match the signature
 * for the event.
 */
template<typename E, typename = typename E::signature>
struct callback_wrappers;

template<typename E, typename R, typename... Args>
struct callback_wrappers<E, R(Args...)> {
	/**
	 * wrapper for object passed via userptr, and member function called for
	 * event.
	 */
	template<typename T>
	static R member_fn(userp_to_voidp<Args>... args) noexcept
	{
		auto event = E(args...);
		auto state = static_cast<T*>(get_userptr<Args...>(args...));
		return state->on(event);
	}

	/**
	 * wrapper for no userptr, and static function of T is called for event.
	 */
	template<typename T>
	static R static_fn(userp_to_voidp<Args>... args) noexcept
	{
		auto event = E(args...);
		return T::on(event);
	}

	/**
	 * wrapper for when userptr is custom data passed to static function of
	 * T for event.
	 */
	template<typename T, typename D>
	static R static_fn(userp_to_voidp<Args>... args) noexcept
	{
		auto event = E(args...);
		auto state = static_cast<D*>(get_userptr<Args...>(args...));
		return T::on(event, to_ref_or_ptr(state));
	}

	template<typename T, T fptr>
	static R function_pointer(userp_to_voidp<Args>... args) noexcept
	{
		auto event = E(args...);
		return fptr(event);
	}

	template<typename T, T fptr, typename D>
	static R function_pointer(userp_to_voidp<Args>... args) noexcept
	{
		auto event = E(args...);
		auto state = static_cast<D*>(get_userptr<Args...>(args...));
		return fptr(event, to_ref_or_ptr(state));
	}

	template<typename T>
	static R functor(userp_to_voidp<Args>... args) noexcept
	{
		T fn;
		auto event = E(args...);
		return fn(event);
	}

	template<typename T, typename D>
	static R functor(userp_to_voidp<Args>... args) noexcept
	{
		T fn;
		auto event = E(args...);
		auto state = static_cast<D*>(get_userptr<Args...>(args...));
		return fn(event, to_ref_or_ptr(state));
	}
};

} // namespace detail
namespace detail { // detectors

// detects t.on(e);
template<typename E, typename T>
using detect_member_fn =
	decltype(void(std::declval<T>().on(std::declval<E>())));

// detects T::on(e);
template<typename E, typename T>
using detect_static_fn =
	decltype(void(T::on(std::declval<E>())));

// detects T::on(e, &d) or T::on(e, d)
template<typename E, typename T, typename D>
using detect_static_userptr_fn =
	decltype(void(T::on(std::declval<E>(), to_ref_or_ptr(std::declval<D*>()))));

} // namespace detail

/*
 * Wrapper metafunctions, returns valid function pointer or nullptr depending on
 * whether the type can be handled or not.
 */

/**
 * Default wrapper case for non-detected functions
 */
template<typename E>
struct wrap_nullptr {
	static constexpr signature<E>* value = nullptr;
};

template<typename E, typename T, typename = void>
struct wrap_member_fn : wrap_nullptr<E> {};

template<typename E, typename T, typename = void>
struct wrap_static_fn : wrap_nullptr<E> {};

template<typename E, typename T, typename D, typename = void>
struct wrap_static_userptr_fn : wrap_nullptr<E> {};

/*
 * Specializations
 */

template<typename E, typename T>
struct wrap_member_fn<E, T, detail::detect_member_fn<E, T>> {
	static constexpr signature<E>* value =
		&detail::callback_wrappers<E>::template member_fn<T>;
};

template<typename E, typename T>
struct wrap_static_fn<E, T, detail::detect_static_fn<E, T>> {
	static constexpr signature<E>* value =
		&detail::callback_wrappers<E>::template static_fn<T>;
};

template<typename E, typename T, typename D>
struct wrap_static_userptr_fn<E, T, D, detail::detect_static_userptr_fn<E, T, D>> {
	static constexpr signature<E>* value =
		&detail::callback_wrappers<E>::template static_fn<T, D>;
};

template<typename E, typename T, T fptr>
struct wrap_function {
	static constexpr signature<E>* value =
		&detail::callback_wrappers<E>::template function_pointer<T, fptr>;
};

template<typename E, typename T, T fptr, typename D>
struct wrap_function_userptr {
	static constexpr signature<E>* value =
		&detail::callback_wrappers<E>::template function_pointer<T, fptr, D>;
};

} // namespace callback_wrapper
#endif // CALLBACK_WRAPPER_HPP
