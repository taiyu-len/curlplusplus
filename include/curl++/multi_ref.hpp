#ifndef CURLPLUSPLUS_MULTI_REF_HPP
#define CURLPLUSPLUS_MULTI_REF_HPP
#include "curl++/easy.hpp"
#include "curl++/multi_opt.hpp"
#include "curl++/types.hpp"     // for mcode
#include <curl/curl.h>          // for CURLM*
namespace curl {
struct multi_ref
{
	multi_ref(CURLM *) noexcept;

	void add_handle(easy_ref);
	void assign(socket_t, void* data);
	void socket_action(socket_t, int ev_bitmask, int* left);

	/** Read next message from multi handle.
	 */
	struct message;
	auto info_read() -> message;

	/** Set a curl multi option.
	 * @pre handle != nullptr
	 * @throws curl::mcode
	 */
	template<CURLMoption o, typename T>
	inline void set(detail::multi_option<o, T> x);
protected:
	CURLM *handle;
};

struct multi_ref::message
{
	CURLMSG msg;
	easy_ref easy_handle;
	union {
		void* whatever;
		code result;
	} data;
};

template<CURLMoption o, typename T>
void multi_ref::set(detail::multi_option<o, T> x)
{
	invoke(curl_multi_setopt, handle, o, x.value);
}
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_REF_HPP


