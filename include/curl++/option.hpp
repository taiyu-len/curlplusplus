#ifndef CURLPLUSPLUS_OPTION_HPP
#define CURLPLUSPLUS_OPTION_HPP
#include "curl++/detail/extract_function.hpp"
#include <curl/curl.h>
#include <string>
namespace curl {
namespace detail { /* option_base, option_enum */
/** Base template for translating c++ types to curl compatible option types on
 * construction.
 * @param O the type of option being set.
 * @param option the option to set.
 * @param T The c++ type.
 */
template<typename O, O option, typename T>
struct option_base
{
	explicit option_base(T x): value(x) {};
	T value;
};
template<typename O, O option>
struct option_base<O, option, std::string>
{
	explicit option_base(std::string const& x): value(x.c_str()) {};
	explicit option_base(const char *x): value(x) {};
	const char* value;
};
template<typename O, O option>
struct option_base<O, option, bool>
{
	explicit option_base(bool x): value(static_cast<long>(x)) {};
	long value;
};
/** Base class representing curl options with limited number of enum values.
 * @param O the type of option being set
 * @param option The option to set.
 * @param val the value of the option
 */
template<typename O, O option, long val>
struct option_enum
{
	static constexpr long value = val;
};
} // namespace detail
namespace option { /* Event handler template */
/** Template class that extracts member functoin poitners and data from a type
 * on construction per event.
 * @param E the event being handled.
 */
template<class E>
struct handler
{
	template<typename T>
	handler(T* x) noexcept
	: fptr(detail::extract_mem_fn<E, T>::fptr())
	, data(static_cast<void*>(x))
	{ /* NOOP */ }

	// Functions to set funtcion and data pointer for the event E.
	// specialized in source files.
	void easy(CURL *) const noexcept;
	void multi(CURLM *) const noexcept;
private:
	// type erased function pointer taking void*
	typename detail::event_fn<E>::signature* fptr;
	// type erased data passed to
	void* data;
};
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_OPTION_HPP