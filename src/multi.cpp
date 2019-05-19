#include "curl++/invoke.hpp"
#include "curl++/multi.hpp"
#include <stdexcept> // for runtime_error
#include <utility> // for exchange

namespace curl { // multi_ref

multi_ref::multi_ref(CURLM *handle) noexcept
: handle(handle)
{}

void multi_ref::add_handle(easy_ref ref)
{
	CURL* raw_easy_handle = ref.handle;
	invoke(curl_multi_add_handle, handle, raw_easy_handle);
}

} // namespace curl
namespace curl { // multi_handle
multi_handle::multi_handle()
: multi_ref(curl_multi_init())
{
	if (handle == nullptr)
	{
		throw std::runtime_error("Curl::multi_handle failed to initialize");
	}
}

multi_handle::multi_handle(multi_handle &&x) noexcept
: multi_ref(std::exchange(x.handle, nullptr))
{}

multi_handle& multi_handle::operator=(multi_handle &&x) noexcept
{
	curl_multi_cleanup(handle);
	handle = std::exchange(x.handle, nullptr);
	return *this;
}

multi_handle::~multi_handle() noexcept
{
	curl_multi_cleanup(handle);
}
} // namespace curl
