#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
namespace curl {
namespace detail { /* event_fn */
/* templated object to specialize to obtain function pointer for event and type.
 *
 * example specialization
 * template<> struct event_fn<EVENT> {
 *   template<typename T>
 *   static int invoke(void *x, int a) noexcept {
 *     return static_cast<T*>(x)->handle(EVENT{a});
 *   }
 *   using signature = int(void*, int);
 * };
 */
template<typename E>
struct event_fn;
} // namespace detail
namespace detail { /* extract_mem_fn */
/** Template for extracting member functions from T for handling event E.
 * @param E The event being handled.
 * @param T The type handling the function.
 * @param _ The parameter used to specialize types that can handle the event.
 *
 * default version returns nullptr.
 */
template<typename E, typename T, typename = void>
struct extract_mem_fn
{
	static constexpr
	typename event_fn<E>::signature* fptr() noexcept
	{
		return nullptr;
	}
};

// used to determine if T can handle event E.
template<typename E, typename T>
using can_handle = decltype(std::declval<T>().handle(std::declval<E>()), void());

/** Specialized version for when T has a function that handles E.
 */
template<typename E, typename T>
struct extract_mem_fn<E, T, can_handle<E, T>>
{
	static constexpr
	typename event_fn<E>::signature* fptr() noexcept
	{
		return &event_fn<E>::template invoke<T>;
	}
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

