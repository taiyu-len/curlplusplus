#ifndef CURLPLUSPLUS_OPTION_HPP
#define CURLPLUSPLUS_OPTION_HPP
#include "types.hpp"
#include "handle_base.hpp"
#include <curl/curl.h>
#include <string>
namespace curl {
namespace option {
namespace detail {

/**
 * Base template for translating c++ types to curl compatible option types on
 * construction.
 *
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

template<typename O, O option, typename T>
struct option_base<O, option, curl::detail::handle_base<T>>
{
	explicit option_base(curl::detail::handle_base<T>& x): value(x.raw()) {};
	T value;
};

/**
 * Type flag for bitflag options.
 */
struct bit_flag_option {};

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

template<typename O, O option>
struct option_base<O, option, curl::error_buffer>
{
	explicit option_base(curl::error_buffer& x):value(&x[0]) {};
	char* value;
};

} // namespace detail
} // namespace option
} // namespace curl
#endif // CURLPLUSPLUS_OPTION_HPP
