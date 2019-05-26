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

void multi_ref::assign(socket_t sockfd, void* data)
{
	invoke(curl_multi_assign, handle, sockfd, data);
}

int multi_ref::socket_action(socket_t sockfd, int ev_bitmask)
{
	int running_handles;
	invoke(curl_multi_socket_action, handle, sockfd, ev_bitmask,
	       &running_handles);
	return running_handles;
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
