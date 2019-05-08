#ifndef CURLPLUSPLUS_EASY_REF_HPP
#define CURLPLUSPLUS_EASY_REF_HPP
#include "curl++/easy_info.hpp"
#include "curl++/easy_opt.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>
namespace curl {
/* Lightweight non-owning reference to a curl easy handle.
 * Allows setting options, getting information, and a most operations on an easy
 * handle.
 * settings regarding event handling are not handled by this.
 */
struct easy_ref
{
	/** Take a handle as reference */
	easy_ref(CURL *handle) noexcept;

	/** Pause or resume receiving or sending data.
	 * @param flag whether to pause or resume transfer
	 * see curl_easy_pause
	 */
	auto pause(curl::pause flag) noexcept -> curl::code;

	auto perform() noexcept -> curl::code;

	/** Set Easy handle options.
	 * example: @code easy.setopt(url{"www.example.com"}); @endcode
	 */
	template<CURLoption o, typename T, long v>
	code set(detail::easy_option<o, T, v> x) noexcept
	{
		return curl_easy_setopt(handle, o, x.value);
	}

	/** Get request info.
	 * example: @code easy.getinfo(url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	T get(info::info<i, T> x) const noexcept
	{
		typename info::info<i, T>::value_type y;
		curl_easy_getinfo(handle, i, &y);
		return x(y);
	}
protected:
	CURL* handle;
};
} // namespace curl
#endif // CURLPLUSPLUS_EASY_REF_HPP

