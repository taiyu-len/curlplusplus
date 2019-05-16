#include "curl++/multi_ref.hpp"
#include "curl++/detail/invoke.hpp"
namespace curl {
multi_ref::multi_ref(CURLM *handle) noexcept
: handle(handle)
{
	// NOOP
}

void multi_ref::add_handle(easy_ref easy_h)
{
	CURL* easy_rh = easy_h.handle;
	detail::invoke(curl_multi_add_handle, handle, easy_rh);
}

} // namespace curl
