#ifndef CURLPLUSPLUS_TYPES_HPP
#define CURLPLUSPLUS_TYPES_HPP
#include <curl/curl.h>
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

//@{
/** callback types for curl easy handles */
template<class T> using cleanup_cb  = int(T*);
template<class T> using debug_cb    = int(CURL*, infotype, char*, size_t, T*);
template<class T> using header_cb   = size_t(char *, size_t, size_t, T*);
template<class T> using read_cb     = size_t(char *, size_t, size_t, T*);
template<class T> using seek_cb     = int(T*, off_t, int);
template<class T> using write_cb    = size_t(char *, size_t, size_t, T*);
template<class T> using xferinfo_cb = int(T*, off_t, off_t, off_t, off_t);
} // namespace curl

#endif // CURLPLUSPLUS_TYPES_HPP

