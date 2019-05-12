#ifndef CURLPLUSPLUS_MULTI_REF_HPP
#define CURLPLUSPLUS_MULTI_REF_HPP
#include "curl++/multi_opt.hpp"    // for multi_option
#include "curl++/types.hpp"        // for mcode
#include <curl/curl.h>             // for CURLM*
namespace curl {
struct easy_ref;
struct multi_ref
{
	multi_ref(CURLM *);

	void add_handle(easy_ref);
	void assign(socket_t, void* data);
	void socket_action(socket_t, int ev_bitmask, int* left);

	/** Set a curl multi option.
	 * @pre handle != nullptr
	 * @throws curl::mcode
	 */
	template<CURLMoption o, typename T>
	void set(detail::multi_option<o, T> x);
protected:
	CURLM *handle;
};

template<CURLMoption o, typename T>
void multi_ref::set(detail::multi_option<o, T> x)
{
	auto ec = curl::mcode(curl_multi_setopt(handle, o, x.value));
	if (ec) throw ec;
}
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_REF_HPP


