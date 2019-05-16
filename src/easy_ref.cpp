#include "curl++/easy_ref.hpp"
#include "curl++/detail/invoke.hpp"
namespace curl {
easy_ref::easy_ref(CURL* handle) noexcept
: handle(handle)
{
	// NOOP
}

easy_ref::operator bool() const noexcept
{
	return handle != nullptr;
}

void easy_ref::pause(curl::pause flag)
{
	detail::invoke(curl_easy_pause, handle, static_cast<long>(flag));
}

void easy_ref::perform()
{
	detail::invoke(curl_easy_perform, handle);
}
} // namespace curl


