#ifndef CURLPLUSPLUS_UTIL_HPP
#define CURLPLUSPLUS_UTIL_HPP
#include <curl/curl.h>
#include <string>
#include <type_traits>
namespace curl {
//@{
/// simple typedefs of curl types
using infotype = curl_infotype;
using off_t    = curl_off_t;
//@}

/// Wrapper around CURLcode type
struct code
{
	code(CURLcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLcode value = CURLE_OK;
};
struct mcode
{
	mcode(CURLMcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLMcode value = CURLM_OK;
};
struct shcode
{
	shcode(CURLSHcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLSHcode value = CURLSHE_OK;
};
struct ucode
{
	ucode(CURLUcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLUcode value = CURLUE_OK;
};

/* Template machinery to translate between user facing argument types, and
 * internal curl types.
 */
namespace detail {
template<typename O, typename I>
struct translate_base
{
	using outer_t = O;
	using inner_t = I;
	enum { allow_inner = false };
	static inner_t to_inner(outer_t const& x) noexcept
	{
		return static_cast<inner_t>(x);
	}
	static outer_t to_outer(inner_t const& x) noexcept
	{
		return static_cast<outer_t>(x);
	}
};

template<>
struct translate_base<std::string, const char*>
{
	using outer_t = std::string;
	using inner_t = const char*;
	enum { allow_inner = true };
	static inner_t to_inner(outer_t const& x) noexcept
	{
		return x.c_str();
	}
	static outer_t to_outer(inner_t x) noexcept
	{
		return static_cast<outer_t>(x);
	}
};

template<typename O, typename = void>
struct translate: public translate_base<O, O> {};

// Specialize std::string <-> const char*
template<>
struct translate<std::string, void>: translate_base<std::string, const char*>
{
	enum { allow_inner = true };
};

// Specialize bool <-> long
template<>
struct translate<bool, void>: translate_base<bool, long> {};

// Specialize enums <-> underlying type
template<typename T>
struct translate<T, std::enable_if_t<std::is_enum<T>::value>>
: translate_base<T, std::underlying_type_t<T>> {};
} // namespace detail

} // namespace curl
#endif // CURLPLUSPLUS_UTIL_HPP


