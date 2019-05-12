#include "curl++/easy_ref.hpp"
namespace curl {
easy_ref::easy_ref(CURL* handle) noexcept
: handle(handle)
{
	// NOOP
}

void easy_ref::pause(curl::pause flag)
{
	auto ec = curl::code(curl_easy_pause(handle, static_cast<long>(flag)));
	if (ec) throw ec;
}

void easy_ref::perform()
{
	auto ec = curl::code(curl_easy_perform(handle));
	if (ec) throw ec;
}
} // namespace curl


