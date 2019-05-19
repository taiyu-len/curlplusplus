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

namespace option { // event handler specializations

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

// Default definition for setting callbacks.
template<typename E>
void handler<E>::easy(CURL* handle, void* data) const noexcept
{
	curl_easy_setopt(handle, opts<E>::func, fptr);
	curl_easy_setopt(handle, opts<E>::data, fptr ? data : NULL);
}

// explicit instantiation for these events.
template void handler< easy_ref::debug    >::easy(CURL*, void*) const noexcept;
template void handler< easy_ref::header   >::easy(CURL*, void*) const noexcept;
template void handler< easy_ref::read     >::easy(CURL*, void*) const noexcept;
template void handler< easy_ref::seek     >::easy(CURL*, void*) const noexcept;
template void handler< easy_ref::progress >::easy(CURL*, void*) const noexcept;

// Specialized version of write, to set it to a no-op
static size_t nowrite(void*, size_t x, size_t y, void*) noexcept
{
	return x*y;
}

template<>
void handler<easy_ref::write>::easy(CURL *handle, void* data) const noexcept
{
	if (fptr) {
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fptr);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
	} else {
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &nowrite);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, NULL);
	}
}

} // namespace option

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

