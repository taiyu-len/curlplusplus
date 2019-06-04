#ifndef CURLPLUSPLUS_SHARE_EVENTS_HPP
#define CURLPLUSPLUS_SHARE_EVENTS_HPP
#include "share.hpp"
#include "easy.hpp"
namespace curl {

struct share::lock {
	using signature = void(CURL*, ::curl_lock_data, ::curl_lock_access, void*);
	static constexpr CURLSHoption FUNC = CURLSHOPT_LOCKFUNC;
	static constexpr CURLSHoption DATA = CURLSHOPT_USERDATA;

	lock( CURL* handle, ::curl_lock_data data, ::curl_lock_access access
	    , void*)
	: handle(handle)
	, data(static_cast<share::lock_data>(data))
	, access(static_cast<share::lock_access>(access))
	{}

	static auto dataptr( CURL*, ::curl_lock_data, ::curl_lock_access
	                   , void* x) noexcept -> void*
	{
		return x;
	}

	easy_ref           handle;
	share::lock_data   data;
	share::lock_access access;
};

struct share::unlock {
	using signature = void(CURL*, ::curl_lock_data, void*);
	static constexpr CURLSHoption FUNC = CURLSHOPT_UNLOCKFUNC;
	static constexpr CURLSHoption DATA = CURLSHOPT_USERDATA;

	unlock(CURL* handle, ::curl_lock_data data, void*)
	: handle(handle)
	, data(static_cast<share::lock_data>(data))
	{}

	static auto dataptr(CURL*, ::curl_lock_data, void* x) noexcept -> void*
	{
		return x;
	}

	easy_ref           handle;
	share::lock_data   data;
};

} // namespace curl

#endif // CURLPLUSPLUS_SHARE_EVENTS_HPP
