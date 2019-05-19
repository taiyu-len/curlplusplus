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
	using signature = typename E::signature;

	/** Extract member function from T to use as handler for event E.
	 * @param T The type containing handler functions, and datapointer type.
	 */
	template<typename T>
	static constexpr auto from_mem_fn() noexcept -> handler
	{
		return detail::extract_mem_fn<E, T>::fptr();
	}

	/** Extract static function from T to use as handler for event E.
	 * @param T The type containing handler functions
	 */
	template<typename T>
	static constexpr auto from_static_fn() noexcept -> handler
	{
		return detail::extract_static_fn<E, T>::fptr();
	}

	/** Extract static function from T to use as handler for event E with
	 * data D.
	 * @param T The type containing the handler functions.
	 * @param D The data pointer type
	 */
	template<typename T, typename D>
	static constexpr auto from_static_fn() noexcept -> handler
	{
		return detail::extract_static_fn_with_data<E, T, D>::fptr();
	}

	// Checks if fptr is vlid
	constexpr explicit operator bool() const noexcept
	{
		return fptr != nullptr;
	}

	/** Functions which set the appropriate options for the given handle.
	 * Specialized in source files where events are defined
	 */
	void easy(CURL *, void*) const noexcept;
	void multi(CURLM *, void*) const noexcept;
private:
	/** Construct handler object from function pointer and data pointer. */
	constexpr handler(signature* fptr) noexcept
	: fptr(fptr)
	{}

	// passed to CURLOPT_****FUNCTION
	signature* fptr;
};
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_OPTION_HPP
