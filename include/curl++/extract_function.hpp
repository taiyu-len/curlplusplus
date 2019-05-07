#ifndef CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#define CURLPLUSPLUS_EXTRACT_FUNCTION_HPP
#include <cstddef>
#include <type_traits>
#include <curl/curl.h>
namespace curl {
namespace detail { /* event_fn */
/* templated object to specialize to obtain function pointer for event and type.
 *
 * example specialization
 * template<> struct event_fn<EVENT> {
 *   template<typename T>
 *   static return_t invoke(void *x, int a) noexcept {
 *     return static_cast<T*>(x)->handle(EVENT{a});
 *   }
 *   using signature = decltype(&invoke<void>);
 * };
 */
template<typename E>
struct event_fn;
} // namespace detail
namespace detail { /* extract_mem_fn */
/* Extracts the member function pointer for the event from T.
 * default version returns null
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

template<typename E, typename T>
using can_handle = decltype(std::declval<T>().handle(std::declval<E>()), void());

/* Specialized version for when T has a function that handles E
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
namespace detail { /* extract_fn */
/* Object that constructs data and function pointer for event from T* */
template<class E>
struct extract_fn
{
	template<typename T>
	extract_fn(T* x) noexcept
	: fptr(detail::extract_mem_fn<E, T>::fptr())
	, data(static_cast<void*>(x))
	{ /* NOOP */ }

	// TODO: static member version with extra data argument

	// sets the handler for this event for the curl object.
	// defined in source files per event
	void easy(CURL *) const noexcept;
	void multi(CURLM *) const noexcept;
private:
	// type erased function pointer taking void*
	typename event_fn<E>::signature* fptr;
	// type erased data passed to
	void* data;
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_EXTRACT_FUNCTION_HPP

