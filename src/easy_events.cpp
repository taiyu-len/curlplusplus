#include "curl++/easy_events.hpp"
#include "curl++/option.hpp" // for handler

namespace curl {
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
} // namespace

template<typename E>
void handler<E>::easy(CURL* handle) const noexcept
{
	curl_easy_setopt(handle, opts<E>::func, fptr);
	curl_easy_setopt(handle, opts<E>::data, fptr ? data : nullptr);
}

template void handler<easy_ref::debug>::easy(CURL*) const noexcept;
template void handler<easy_ref::header>::easy(CURL*) const noexcept;
template void handler<easy_ref::read>::easy(CURL*) const noexcept;
template void handler<easy_ref::seek>::easy(CURL*) const noexcept;
template void handler<easy_ref::progress>::easy(CURL*) const noexcept;

// Specialized version of write, to set it to a no-op
static size_t nowrite(void*, size_t x, size_t y, void*) noexcept
{
	return x*y;
}
template<>
void handler<easy_ref::write>::easy(CURL *handle) const noexcept
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

