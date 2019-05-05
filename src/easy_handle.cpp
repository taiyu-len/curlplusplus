#include "curl++/easy_handle.hpp"
#include "private_t.hpp"
#include <stdexcept>

namespace curl {
easy_handle::easy_handle()
: easy_ref(curl_easy_init())
{
	if (handle == nullptr)
	{
		throw std::runtime_error("curl::easy_handle failed to initialize");
	}
}

easy_handle::easy_handle(easy_handle &&x) noexcept
: easy_ref(x.handle)
{
	x.handle = nullptr;
}

easy_handle& easy_handle::operator=(easy_handle && x) noexcept
{
	curl_easy_cleanup(handle);
	handle = x.handle;
	x.handle = nullptr;
	return *this;
}

easy_handle::~easy_handle() noexcept
{
	if (handle)
	{
		detail::private_t* p;
		curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
		if (p)
		{
			delete p;
		}
		curl_easy_cleanup(handle);
	}
}

void easy_handle::cleanup_function(cleanup_cb<void>* fn, void* data) noexcept
{
	detail::private_t* p;
	curl_easy_getinfo(handle, CURLINFO_PRIVATE, &p);
	if (p == nullptr)
	{
		p = new detail::private_t();
		curl_easy_setopt(handle, CURLOPT_PRIVATE, p);
	}
	p->fn = fn;
	p->ptr = data;
}

void easy_handle::debug_function(debug_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_DEBUGDATA, data);
}

void easy_handle::header_function(header_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_HEADERDATA, data);
}

void easy_handle::read_function(read_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_READFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_READDATA, data);
}

void easy_handle::seek_function(seek_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_SEEKFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_SEEKDATA, data);
}

void easy_handle::write_function(write_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
}

void easy_handle::xferinfo_function(xferinfo_cb<void>* fn, void* data) noexcept
{
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, fn);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, data);
}
} // namespace curl
