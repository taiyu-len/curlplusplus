#ifndef CURLPLUSPLUS_EASY_REF_HPP
#define CURLPLUSPLUS_EASY_REF_HPP
#include "curl++/easy_info.hpp" // for easy_info
#include "curl++/easy_opt.hpp"  // for easy_option
#include "curl++/types.hpp"     // 
#include <curl/curl.h>          // for CURL*
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
	template<CURLoption o, typename T>
	auto set(detail::easy_option<o, T>) noexcept -> curl::code;

	/** Get request info.
	 * example: @code easy.getinfo(url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	auto get(info::info<i, T>) const noexcept -> T;
protected:
	CURL* handle;
};

template<CURLoption o, typename T>
auto easy_ref::set(detail::easy_option<o, T> x) noexcept -> curl::code
{
	return curl_easy_setopt(handle, o, x.value);
}

template<CURLINFO i, typename T>
auto easy_ref::get(info::info<i, T> x) const noexcept -> T
{
	typename info::info<i, T>::value_type y;
	curl_easy_getinfo(handle, i, &y);
	return x(y);
}

} // namespace curl
#endif // CURLPLUSPLUS_EASY_REF_HPP

