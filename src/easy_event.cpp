#include "curl++/easy_event.hpp"
#include "private_t.hpp"
namespace curl {
namespace detail {
namespace {
template<typename E> struct opts;
#define OPTION(n, N) \
template<> struct opts<event::n> { \
	static constexpr CURLoption func = CURLOPT_## N ##FUNCTION; \
	static constexpr CURLoption data = CURLOPT_## N ##DATA; \
}
OPTION(debug, DEBUG);
OPTION(header, HEADER);
OPTION(read, READ);
OPTION(seek, SEEK);
OPTION(write, WRITE);
OPTION(progress, XFERINFO);

} // namespace

template<typename E>
void callback<E>::set_handler(CURL* handle) const noexcept
{
	curl_easy_setopt(handle, opts<E>::func, fptr);
	curl_easy_setopt(handle, opts<E>::data, fptr ? data : nullptr);
}

// TODO? add specialized version for debug/progress that set VERBOSE/PROGRESS
// option
template void callback<event::debug>::set_handler(CURL*) const noexcept;
template void callback<event::header>::set_handler(CURL*)const noexcept;
template void callback<event::read>::set_handler(CURL*)const noexcept;
template void callback<event::seek>::set_handler(CURL*)const noexcept;
template void callback<event::write>::set_handler(CURL*)const noexcept;
template void callback<event::progress>::set_handler(CURL*)const noexcept;

// specialized version for cleaunp
template<>
void callback<event::cleanup>::set_handler(CURL *handle) const noexcept
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


