#include "curl++/multi_handle.hpp"
#include <stdexcept> // for runtime_error
#include <utility> // for exchange
namespace curl {
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
{
	// NOOP
}

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
