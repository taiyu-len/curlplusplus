#ifndef CURLPLUSPLUS_INFO_READ_HPP
#define CURLPLUSPLUS_INFO_READ_HPP
#include "curl++/easy.hpp"
#include <curl/curl.h>
namespace curl {
/**
 * message object returned from info_read for each easy request.
 */
struct info_read_message {
	CURLMSG  msg;
	easy_ref ref;
	code     result   = CURLE_OK;
	void*    whatever = nullptr;

	auto operator->() noexcept -> info_read_message*
	{
		return this;
	}
};

/**
 * Iterator for iterating over messages from a multi_handle.
 */
struct info_read_iterator {
	using iterator_category = std::forward_iterator_tag;
	using value_type        = CURLMsg;
	using difference_type   = int;
	using reference         = info_read_message;
	using pointer           = info_read_message;

	/**
	 * Constructs end().
	 */
	info_read_iterator() noexcept = default;

	/**
	 * Constructs begin().
	 */
	info_read_iterator(CURLM *handle) noexcept
	: _handle(handle)
	{
		++*this;
	}

	bool operator!=(info_read_iterator x) noexcept
	{
		return _message != x._message;
	}

	/**
	 * Sets iterator to refer to next message.
	 */
	auto operator++() noexcept -> info_read_iterator&
	{
		_message = curl_multi_info_read(_handle, &_remaining);
		return *this;
	}

	/**
	 * Access current message.
	 */
	auto operator*()  noexcept -> reference
	{
		info_read_message m;
		m.msg = _message->msg;
		m.ref = _message->easy_handle;
		if (m.msg == CURLMSG_DONE) {
			m.result = _message->data.result;
		} else {
			m.whatever = _message->data.whatever;
		}
		return m;
	}

	/**
	 * Access current message.
	 */
	auto operator->() noexcept -> pointer
	{
		return **this;
	}

	/**
	 * return how many remaining messages there are.
	 */
	auto remaining() const noexcept -> int
	{
		return _remaining;
	}

private:
	CURLM*   _handle    = nullptr;
	CURLMsg* _message   = nullptr;
	int      _remaining = 0;
};

/**
 * Iterable object for info_read messages
 */
struct info_read_proxy {
	/**
	 * Construct from multi handle.
	 */
	info_read_proxy(CURLM * h)
	: _handle(h)
	{}

	auto begin() -> info_read_iterator { return {_handle}; }
	auto end()   -> info_read_iterator { return {}; }

private:
	CURLM*   _handle;
};
} // namespace curl

#endif // CURLPLUSPLUS_INFO_READ_HPP
