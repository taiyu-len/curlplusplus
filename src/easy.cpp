#include "curl++/easy.hpp"
#include "curl++/invoke.hpp"
#include "curl++/option.hpp" // for handler
#include <curl/curl.h>       // for curl_easy_cleanup, curl_easy_init
#include <stdexcept>         // for runtime_error
#include <utility>           // for exchange

namespace curl { // easy_ref

easy_ref::easy_ref(CURL* handle) noexcept : handle(handle)
{}

void easy_ref::pause(curl::pause flag)
{
	invoke(curl_easy_pause, handle, static_cast<long>(flag));
}

void easy_ref::perform()
{
	invoke(curl_easy_perform, handle);
}

} // namespace curl
namespace curl { // curl_ref::events

namespace { // gets CURLOPTs per event type
template<typename E> struct opts;
#define OPTION(n, N) \
template<> struct opts<easy_ref::n> { \
	static constexpr CURLoption func = CURLOPT_## N ##FUNCTION; \
	static constexpr CURLoption data = CURLOPT_## N ##DATA; \
}
OPTION(debug, DEBUG);
OPTION(header, HEADER);
OPTION(progress, XFERINFO);
OPTION(read, READ);
OPTION(seek, SEEK);
#undef OPTION
} // namespace

#define DEFINE_DEFAULT_SETOPT(EVENT) \
void easy_ref::EVENT::setopt(CURL* handle, signature* fptr, void* data) noexcept \
{ \
	curl_easy_setopt(handle, opts<EVENT>::func, fptr); \
	curl_easy_setopt(handle, opts<EVENT>::data, fptr ? data : NULL); \
}
DEFINE_DEFAULT_SETOPT(debug);
DEFINE_DEFAULT_SETOPT(header);
DEFINE_DEFAULT_SETOPT(progress);
DEFINE_DEFAULT_SETOPT(read);
DEFINE_DEFAULT_SETOPT(seek);
#undef DEFINE_DEFAULT_SETOPT


// Specialized version of write, to set it to a no-op
static size_t nowrite(char*, size_t x, size_t y, void*) noexcept
{
	return x*y;
}

void easy_ref::write::setopt(CURL* handle, signature* fptr, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fptr ? fptr : &nowrite);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA,     fptr ? data : NULL);
}

} // namespace curl
namespace curl { // easy_handle

easy_handle::easy_handle() : easy_ref(curl_easy_init())
{
	if (handle == nullptr)
	{
		throw std::runtime_error("curl::easy_handle failed to initialize");
	}
}

easy_handle::easy_handle(easy_handle &&x) noexcept
	: easy_ref(std::exchange(x.handle, nullptr))
{}

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

