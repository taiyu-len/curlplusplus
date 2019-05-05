#include "curl++/easy_ref.hpp"
#include "private_t.hpp"

namespace curl {
easy_ref::easy_ref(CURL* handle) noexcept
: handle(handle)
{
	// NOOP
}

auto easy_ref::pause(curl::pause flag) const noexcept -> curl::code
{
	return curl_easy_pause(handle, flag);
}

void easy_ref::userdata(void *data) noexcept
{
	detail::private_t* p;
	curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
	if (p == nullptr)
	{
		p = new detail::private_t();
		curl_easy_setopt(handle, CURLOPT_PRIVATE, p);
	}
	p->data = data;
}

void* easy_ref::userdata() const noexcept
{
	detail::private_t* p;
	curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
	return p ? p->data : nullptr;
}
} // namespace curl


