#ifndef CURLPLUSPLUS_INFO_HPP
#define CURLPLUSPLUS_INFO_HPP
#include "invoke.hpp"
#include <chrono>
#include <curl/curl.h>
#include <string>

namespace curl {
namespace detail {
/**
 * Base info type.
 * @param 1 the info value.
 * @param T the user facing argument type.
 */
template<typename T>
struct info {
	/**
	 * calls getinfo on the given handle for the given info value and
	 * returns the value.
	 */
	static auto getinfo(CURL* handle, CURLINFO info) -> T
	{
		return invoke_r<T>(::curl_easy_getinfo, handle, info);
	}
};

template<>
struct info<std::string> {
	static auto getinfo(CURL* handle, CURLINFO info) -> std::string
	{
		return invoke_r<const char*>(::curl_easy_getinfo, handle, info);
	}
};

template<>
struct info<bool> {
	static auto getinfo(CURL* handle, CURLINFO info) -> bool
	{
		return invoke_r<long>(::curl_easy_getinfo, handle, info);
	}
};

template<class Rep, class Period>
struct info<std::chrono::duration<Rep, Period>> {
	using type = std::chrono::duration<Rep, Period>;

	static auto getinfo(CURL* handle, CURLINFO info) -> type
	{
		return type(invoke_r<curl_off_t>(::curl_easy_getinfo, handle, info));
	}
};
} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_INFO_HPP
