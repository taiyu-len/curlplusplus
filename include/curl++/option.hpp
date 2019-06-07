#ifndef CURLPLUSPLUS_OPTION_HPP
#define CURLPLUSPLUS_OPTION_HPP
#include "types.hpp"
#include "handle_base.hpp"
#include <curl/curl.h>
#include <string>
namespace curl {
namespace detail {

/**
 * Templated class constructed from common c++ and translated to curl compatible
 * c Types on construction.
 *
 * @param T The c++ type.
 */
template<typename T>
struct option {
	constexpr option(T x) noexcept
	: value(x)
	{}

	T value;
};

template<>
struct option<std::string> {
	option(std::string const& x)
	: value(x.c_str())
	{}

	constexpr option(const char *x)
	: value(x)
	{}

	const char* value;
};

template<>
struct option<bool> {
	constexpr option(bool x)
	: value(static_cast<long>(x))
	{}

	long value;
};

// takes easy_ref/multi_ref/share_ref types.
template<typename T>
struct option<handle_base<T>> {
	constexpr option(handle_base<T>& x)
	: value(x.raw())
	{};

	T value;
};

template<>
struct option<error_buffer> {
	option(error_buffer& x)
	: value(&x[0])
	{}

	char* value;
};

/*
 * Wrapper for enum types.
 */
template<typename T>
struct option_enum {};

template<typename T>
struct option<option_enum<T>> {
	constexpr option(T x)
	: value(static_cast<unsigned long>(x))
	{}

	unsigned long value;
};

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_OPTION_HPP
