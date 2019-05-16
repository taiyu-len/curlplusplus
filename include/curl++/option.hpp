#ifndef CURLPLUSPLUS_OPTION_HPP
#define CURLPLUSPLUS_OPTION_HPP
#include "curl++/detail/extract_function.hpp"
#include "curl++/detail/is_detected.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>
#include <string>
namespace curl {
namespace detail { /* bit_flag_option, option_base, option_enum */
struct bit_flag_option {};

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

// Specialization for std::string
template<typename O, O option>
struct option_base<O, option, std::string>
{
	explicit option_base(std::string const& x): value(x.c_str()) {};
	explicit option_base(const char *x): value(x) {};
	const char* value;
};

// Specialization for bool
template<typename O, O option>
struct option_base<O, option, bool>
{
	explicit option_base(bool x): value(static_cast<long>(x)) {};
	long value;
};

// Specialization for bitflags
template<typename O, O option>
struct option_base<O, option, bit_flag_option>
{
	explicit option_base(unsigned long x): value(x) {};
	unsigned long value;
	auto operator|(option_base x) const noexcept -> option_base
	{
		return option_base(value | x.value);
	}
	auto operator|=(option_base x) noexcept -> option_base&
	{
		value |= x.value;
		return *this;
	}
};

// Specializatoin for error_buffers
template<typename O, O option>
struct option_base<O, option, curl::error_buffer>
{
	explicit option_base(curl::error_buffer& x):value(x.data()) {};
	char* value;
};

} // namespace detail

namespace option { /* handler template */
/** Template class that extracts member functoin poitners and data from a type
 * on construction per event.
 * @param E the event being handled.
 */
template<class E>
struct handler
{
	using signature = typename detail::event_info<E>::signature;

	/** Construct handler object from function pointer and data pointer.
	 * May require reinterpret casting fptr, not very safe.
	 */
	template<typename T>
	handler(signature* fptr, T x)
	: fptr(fptr)
	, data(static_cast<void*>(x)) {}

	/** Extract member function from T to use as handler for event E.
	 * @param T The type containing handler functions, and datapointer type.
	 * @param S whether to emit compiler error if T cannot handle E
	 */
	template<bool S, typename T>
	static auto from_mem_fn(T* x) noexcept -> handler
	{
		using detected = detail::is_detected<detail::detect_mem_fn, E, T>;
		static_assert(
			S || detected::value,
			"Cannot extract t.handle(e) member function");
		return handler(
			detail::extract_mem_fn<E, T>::fptr(),
			static_cast<void*>(x));
	}

	/** Extract static function from T to use as handler for event E.
	 * @param T The type containing handler functions
	 * @param S whether to emit compiler error if T cannot handle E
	 */
	template<bool S, typename T>
	static auto from_static_fn() noexcept -> handler
	{
		using detected = detail::is_detected<detail::detect_static_fn, E, T>;
		static_assert(
			S || detected::value,
			"Cannot extract t::handle(e) static function");
		return handler(
			detail::extract_static_fn<E, T>::fptr(),
			nullptr);
	}

	/** Extract static function from T to use as handler for event E with
	 * data D.
	 * @param T The type containing the handler functions.
	 * @param D The data pointer type
	 * @param S whether to emit compiler error if T cannot handle E
	 */
	template<bool S, typename T, typename D>
	static auto from_static_fn(D *x) noexcept -> handler
	{
		using detected = detail::is_detected<detail::detect_static_fn_with_data, E, T, D>;
		static_assert(
			S || detected::value,
			"Cannot extract t::handle(e, d) static function");
		return handler(
			detail::extract_static_fn_with_data<E, T, D>::fptr(),
			static_cast<void*>(x));
	}

	// Functions to set funtcion and data pointer for the event E.
	// specialized in source files.
	void easy(CURL *) const noexcept;
	void multi(CURLM *) const noexcept;
private:
	// type erased function pointer taking void*
	signature* fptr;
	// type erased data passed to
	void* data;
};
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_OPTION_HPP
