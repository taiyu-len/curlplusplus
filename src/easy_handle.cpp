#include "curl++/easy_handle.hpp"
#include "private_t.hpp" // for private_t
#include <curl/curl.h>   // for curl_easy_cleanup, curl_easy_init, CURLINFO_PRIVATE
#include <stdexcept>     // for runtime_error

namespace curl {
easy_handle::easy_handle()
: easy_ref(curl_easy_init())
{
	if (handle == nullptr)
	{
		throw std::runtime_error("curl::easy_handle failed to initialize");
	}
}

easy_handle::easy_handle(easy_handle &&x) noexcept
: easy_ref(x.handle)
{
	x.handle = nullptr;
}

easy_handle& easy_handle::operator=(easy_handle && x) noexcept
{
	curl_easy_cleanup(handle);
	handle = x.handle;
	x.handle = nullptr;
	return *this;
}

easy_handle::~easy_handle() noexcept
{
	if (handle)
	{
		detail::private_t* p;
		curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
		if (p)
		{
			delete p;
		}
		curl_easy_cleanup(handle);
	}
}
} // namespace curl
