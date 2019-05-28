#ifndef CURLPLUSPLUS_DETAIL_EASY_EVENTS_HPP
#define CURLPLUSPLUS_DETAIL_EASY_EVENTS_HPP
#include "curl++/easy.hpp"
#include "curl++/buffer.hpp"

namespace curl {
/**
 * see CURLOPT_WRITEFUNCTION.
 */
struct easy_ref::write : const_buffer {
	using signature = size_t(const char*, size_t, size_t, void*);

	static constexpr CURLoption FUNC = CURLOPT_WRITEFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_WRITEDATA;
	static auto DEFAULT(const char*, size_t s, size_t t, void*) -> size_t
	{
		return s*t;
	}

	write(const char* d, size_t s, size_t t, void*) noexcept
	: const_buffer(d, s*t)
	{};

	static auto dataptr(const char*, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

/**
 * see CURLOPT_HEADERFUNCTION.
 */
struct easy_ref::header : const_buffer {
	using signature = size_t(const char*, size_t, size_t, void*);

	static constexpr CURLoption FUNC = CURLOPT_HEADERFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_HEADERDATA;

	header(const char* d, size_t s, size_t t, void*) noexcept
	: const_buffer(d, s*t)
	{};

	static auto dataptr(const char*, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

/**
 * see CURLOPT_READFUNCTION.
 */
struct easy_ref::read : mutable_buffer {
	using signature = size_t(char*, size_t, size_t, void*);

	static constexpr CURLoption FUNC = CURLOPT_READFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_READDATA;

	read(char* d, size_t s, size_t t, void*) noexcept
	: mutable_buffer(d, s*t)
	{};

	static auto dataptr(char*, size_t, size_t, void* x) noexcept
	{
		return x;
	}
};

/**
 * see CURLOPT_DEBUGFUNCTION.
 */
struct easy_ref::debug : const_buffer {
	using signature = int(CURL*, infotype, char*, size_t, void*);

	static constexpr CURLoption FUNC = CURLOPT_DEBUGFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_DEBUGDATA;

	easy_ref handle;
	infotype type;

	debug(CURL* e, infotype i, const char* c, size_t s, void*) noexcept
	: const_buffer{c, s}
	, handle(e)
	, type(i)
	{}

	static auto dataptr(CURL*, infotype, char*, size_t, void* x) noexcept -> void*
	{
		return x;
	}
};

/**
 * see CURLOPT_SEEKFUNCTION.
 */
struct easy_ref::seek {
	using signature = int(void*, curl_off_t, int);

	static constexpr CURLoption FUNC = CURLOPT_SEEKFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_SEEKDATA;

	curl_off_t offset;
	int origin;

	seek(void*, curl_off_t offset, int origin) noexcept
	: offset(offset)
	, origin(origin)
	{}

	static auto dataptr(void* x, curl_off_t, int) noexcept -> void*
	{
		return x;
	}
};

/**
 * see CURLOPT_XFERINFOFUNCTION.
 */
struct easy_ref::progress {
	using signature = int(void*, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

	static constexpr CURLoption FUNC = CURLOPT_XFERINFOFUNCTION;
	static constexpr CURLoption DATA = CURLOPT_XFERINFODATA;

	curl_off_t dltotal, dlnow, ultotal, ulnow;

	progress(void*, curl_off_t dt, curl_off_t dn, curl_off_t ut,
	         curl_off_t un) noexcept
	: dltotal(dt)
	, dlnow(dn)
	, ultotal(ut)
	, ulnow(un)
	{}

	static auto dataptr(void* x, curl_off_t, curl_off_t,
	                    curl_off_t, curl_off_t) noexcept -> void*
	{
		return x;
	}
};
} // namespace curl

#endif // CURLPLUSPLUS_DETAIL_EASY_EVENTS_HPP
