#include "curl++/easy.hpp"
#include "private_t.hpp"
namespace curl {
namespace detail {
namespace {
template<typename E> struct opts;
#define OPTION(n, N) \
template<> struct opts<easy_event::n> { \
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
void extract_fn<E>::easy(CURL* handle) const noexcept
{
	curl_easy_setopt(handle, opts<E>::func, fptr);
	curl_easy_setopt(handle, opts<E>::data, fptr ? data : nullptr);
}

// TODO? add specialized version for debug/progress that set VERBOSE/PROGRESS
// option
template void extract_fn<easy_event::debug>::easy(CURL*) const noexcept;
template void extract_fn<easy_event::header>::easy(CURL*) const noexcept;
template void extract_fn<easy_event::read>::easy(CURL*) const noexcept;
template void extract_fn<easy_event::seek>::easy(CURL*) const noexcept;
template void extract_fn<easy_event::progress>::easy(CURL*) const noexcept;

// Specialized version of write, to set it to a no-op
size_t nowrite(void*, size_t x, size_t y, void*) noexcept
{
	return x*y;
}
template<>
void extract_fn<easy_event::write>::easy(CURL *handle) const noexcept
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
void extract_fn<easy_event::cleanup>::easy(CURL *handle) const noexcept
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
} // namespace detail
} // namespace curl

