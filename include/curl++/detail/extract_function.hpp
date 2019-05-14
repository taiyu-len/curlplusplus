#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
namespace curl {
namespace detail { /* event_fn */
/** Struct to specialize to obtain function pointer for event and type.
 *
 * example: @code
 * template<> struct event_fn<EVENT> {
 *   template<class T>
 *   using signature = int(T*, int);
 *
 *   template<typename T>
 *   static int invoke(void *x, int a) noexcept {
 *     return static_cast<T*>(x)->handle(EVENT{a});
 *   }
 * };
 * @endcode
 */
template<typename E>
struct event_fn;

} // namespace detail

namespace detail { /* extract_mem_fn */
/** Template for extracting member functions from T for handling event E.
 * @param E The event being handled.
 * @param T The type handling the function.
 * @param _ SFINAE parameter to select specializations.
 *
 * default version returns nullptr.
 */
template<typename E, typename T, typename = void>
struct extract_mem_fn
{
	static constexpr
	typename event_fn<E>::template signature<void>* fptr() noexcept
	{
		return nullptr;
	}
};

template<typename E, typename T>
using detect_can_handle = decltype(std::declval<T>().handle(std::declval<E>()), void());

template<typename E, typename T, typename = void>
struct can_handle : std::false_type {};

template<typename E, typename T>
struct can_handle<E, T, detect_can_handle<E, T>>
: std::true_type {};

/** Specialized version for when T has a function that handles E. */
template<typename E, typename T>
struct extract_mem_fn<E, T, detect_can_handle<E, T>>
{
	static constexpr
	typename event_fn<E>::template signature<void>* fptr() noexcept
	{
		return &event_fn<E>::template invoke<T>;
	}
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

