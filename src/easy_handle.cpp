#include "curl++/easy_handle.hpp"
#include <curl/curl.h>   // for curl_easy_cleanup, curl_easy_init, CURLINFO_PRIVATE
#include <stdexcept>     // for runtime_error
#include <utility>       // for exchange

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
: easy_ref(std::exchange(x.handle, nullptr))
{
	// NOOP
}
easy_handle& easy_handle::operator=(easy_handle && x) noexcept
{
	curl_easy_cleanup(handle);
	handle = std::exchange(x.handle, nullptr);
	return *this;
}
easy_handle::~easy_handle() noexcept
{
	curl_easy_cleanup(handle);
}
} // namespace curl
