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
	 * @throws curl::code
	 * @pre handle != nullptr
	 * see curl_easy_pause
	 */
	void pause(curl::pause flag);

	/** Perform the request.
	 * @throws curl::code
	 * @pre handle != nullptr
	 */
	void perform();

	/** Set Easy handle options.
	 * @param option curl::option::* type
	 * @throws curl::code
	 * @pre handle != nullptr
	 *
	 * example: @code easy.setopt(url{"www.example.com"}); @endcode
	 */
	template<CURLoption o, typename T>
	void set(detail::easy_option<o, T>);

	/** Get request info.
	 * @param option curl::info::* type
	 * @throws curl::code
	 * @pre handle != nullptr.
	 *
	 * example: @code easy.getinfo(url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	auto get(info::info<i, T>) const -> T;
protected:
	CURL* handle;
};

template<CURLoption o, typename T>
void easy_ref::set(detail::easy_option<o, T> x)
{
	curl::code ec = curl_easy_setopt(handle, o, x.value);
	if (ec) throw ec;
}

template<CURLINFO i, typename T>
auto easy_ref::get(info::info<i, T> x) const -> T
{
	typename info::info<i, T>::value_type y;
	curl::code ec = curl_easy_getinfo(handle, i, &y);
	if (ec) throw ec;
	return x(y);
}

} // namespace curl
#endif // CURLPLUSPLUS_EASY_REF_HPP

