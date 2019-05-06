#ifndef CURLPLUSPLUS_TYPES_HPP
#define CURLPLUSPLUS_TYPES_HPP
#include <cstddef>
#include <curl/curl.h>
#include <type_traits>
namespace curl {
// simple typedefs of curl types
using infotype = curl_infotype;
using off_t    = curl_off_t;

// Curl code types
struct code
{
	code(CURLcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLcode value = CURLE_OK;
};

struct mcode
{
	mcode(CURLMcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLMcode value = CURLM_OK;
};

struct shcode
{
	shcode(CURLSHcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLSHcode value = CURLSHE_OK;
};

struct ucode
{
	ucode(CURLUcode value): value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLUcode value = CURLUE_OK;
};

/** Flags used for easy::pause(). */
enum pause : long
{
	recv = CURLPAUSE_RECV,
	send = CURLPAUSE_SEND,
	all  = CURLPAUSE_ALL,
	cont = CURLPAUSE_CONT
};
} // namespace curl

#endif // CURLPLUSPLUS_TYPES_HPP

