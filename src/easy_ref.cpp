#include "curl++/easy_ref.hpp"
namespace curl {
easy_ref::easy_ref(CURL* handle) noexcept
: handle(handle)
{
	// NOOP
}

auto easy_ref::pause(curl::pause flag) noexcept -> curl::code
{
	return curl_easy_pause(handle, flag);
}

auto easy_ref::perform() noexcept -> curl::code
{
	return curl_easy_perform(handle);
}
} // namespace curl


