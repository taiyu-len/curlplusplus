#include "curl++/easy_events.hpp"
#include "curl++/option.hpp" // for handler
#include "private_t.hpp" // for private_t

namespace curl {
namespace option { // event handler specializations
namespace { // gets CURLOPTs per event type
template<typename E> struct opts;
#define OPTION(n, N) \
template<> struct opts<easy_events::n> { \
	static constexpr CURLoption func = CURLOPT_## N ##FUNCTION; \
	static constexpr CURLoption data = CURLOPT_## N ##DATA; \
}
OPTION(debug, DEBUG);
OPTION(header, HEADER);
OPTION(progress, XFERINFO);
OPTION(read, READ);
OPTION(seek, SEEK);
OPTION(write, WRITE);
} // namespace

template<typename E>
void handler<E>::easy(CURL* handle) const noexcept
{
	curl_easy_setopt(handle, opts<E>::func, fptr);
	curl_easy_setopt(handle, opts<E>::data, fptr ? data : nullptr);
}

template void handler<easy_events::debug>::easy(CURL*) const noexcept;
template void handler<easy_events::header>::easy(CURL*) const noexcept;
template void handler<easy_events::read>::easy(CURL*) const noexcept;
template void handler<easy_events::seek>::easy(CURL*) const noexcept;
template void handler<easy_events::progress>::easy(CURL*) const noexcept;

// Specialized version of write, to set it to a no-op
size_t nowrite(void*, size_t x, size_t y, void*) noexcept
{
	return x*y;
}
template<>
void handler<easy_events::write>::easy(CURL *handle) const noexcept
{
	if (fptr) {
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fptr);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
	} else {
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &nowrite);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, NULL);
	}
}

// specialized version for cleaunp
template<>
void handler<easy_events::cleanup>::easy(CURL *handle) const noexcept
{
	detail::private_t* p;
	curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
	if (p == nullptr)
	{
		p = new detail::private_t();
		curl_easy_setopt(handle, CURLOPT_PRIVATE, p);
	}
	p->fptr = fptr;
	p->data = data;
}
} // namespace option
} // namespace curl

