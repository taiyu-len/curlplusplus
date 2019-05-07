#ifndef CURLPLUSPLUS_EASY_REF_HPP
#define CURLPLUSPLUS_EASY_REF_HPP
#include "curl++/easy_info.hpp"
#include "curl++/easy_opt.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>

namespace curl {
/** Light non owning wrapper around a CURL_easy handle.
 *
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
	code set(option::option<o, T> x) noexcept
	{
		return curl_easy_setopt(handle, o, x.value);
	}

	/** Get request info.
	 * example: @code easy.getinfo(url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	T get(info::info<i, T>) const noexcept
	{
		typename info::info<i, T>::inner_t x;
		curl_easy_getinfo(handle, i, &x);
		return info::info<i, T>::to_outer(x);
	}

	//@{
	/** Sets and gets user defined data.
	 */
	void  userdata(void *data) noexcept;
	void* userdata() const noexcept;
	//@}
protected:
	CURL* handle;
};
} // namespace curl
#endif // CURLPLUSPLUS_EASY_REF_HPP

