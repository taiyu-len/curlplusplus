#include "curl++/easy.hpp"
#include "curl++/invoke.hpp"
#include "curl++/option.hpp" // for handler
#include <curl/curl.h>       // for curl_easy_cleanup, curl_easy_init
#include <stdexcept>         // for runtime_error
#include <utility>           // for exchange

namespace curl { // easy_ref

void easy_ref::init()
{
	auto tmp = curl_easy_init();
	if (tmp == nullptr)
	{
		throw std::runtime_error("easy_ref failed to initialize");
	}
	reset(tmp);
}

void easy_ref::reset(CURL* h) noexcept
{
	curl_easy_cleanup(std::exchange(_handle, h));
}

void easy_ref::pause(pause_flags flag)
{
	invoke(curl_easy_pause, _handle, static_cast<long>(flag));
}

void easy_ref::perform()
{
	invoke(curl_easy_perform, _handle);
}

} // namespace curl
namespace curl { // easy_ref::events

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
static auto nowrite(char*, size_t x, size_t y, void*) noexcept -> size_t
{
	return x*y;
}

void easy_ref::write::setopt(CURL* handle, signature* fptr, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fptr ? fptr : &nowrite);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA,     fptr ? data : NULL);
}

} // namespace curl
namespace curl { // easy

easy::easy()
{
	init();
}

easy::easy(easy &&x) noexcept
	: easy_ref(std::exchange(x._handle, nullptr))
{}

auto easy::operator=(easy && x) noexcept -> easy&
{
	reset(std::exchange(x._handle, nullptr));
	return *this;
}

easy::~easy() noexcept
{
	reset();
}

} // namespace curl

