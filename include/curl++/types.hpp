#ifndef CURLPLUSPLUS_TYPES_HPP
#define CURLPLUSPLUS_TYPES_HPP
#include <curl/curl.h>   // for CURLMcode, CURLSHcode, CURLUcode, CURLcode
#include <exception>     // for exception
#include <string>        // for string
namespace curl {
// simple typedefs of curl types
using infotype = curl_infotype;
using socket_t = curl_socket_t;

// error buffer type
struct error_buffer : public std::string {
	error_buffer();
};

// Curl code types
struct code : std::exception {
	code(CURLcode value) noexcept : value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept override;
	CURLcode value = CURLE_OK;
};

struct mcode : std::exception {
	mcode(CURLMcode value) noexcept : value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLMcode value = CURLM_OK;
};

struct shcode : std::exception {
	shcode(CURLSHcode value) noexcept : value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLSHcode value = CURLSHE_OK;
};

struct ucode : std::exception {
	ucode(CURLUcode value) noexcept : value(value) {};
	explicit operator bool() const noexcept;
	const char* what() const noexcept;
	CURLUcode value = CURLUE_OK;
};

} // namespace curl

#endif // CURLPLUSPLUS_TYPES_HPP

