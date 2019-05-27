#include "curl++/invoke.hpp"
#include "curl++/multi.hpp"
#include <stdexcept> // for runtime_error
#include <utility> // for exchange

namespace curl { // multi_ref

void multi_ref::init()
{
	auto tmp = curl_multi_init();
	if (tmp == nullptr)
	{
		throw std::runtime_error("multi handle failed to initialize");
	}
	reset(tmp);
}

void multi_ref::reset(CURLM* h) noexcept
{
	curl_multi_cleanup(std::exchange(_handle, h));
}

void multi_ref::add_handle(easy_ref ref)
{
	CURL* raw_easy_handle = ref._handle;
	invoke(curl_multi_add_handle, _handle, raw_easy_handle);
}

void multi_ref::remove_handle(easy_ref ref)
{
	CURL* raw_easy_handle = ref._handle;
	invoke(curl_multi_remove_handle, _handle, raw_easy_handle);
}

void multi_ref::assign(socket_t sockfd, void* data)
{
	invoke(curl_multi_assign, _handle, sockfd, data);
}

auto multi_ref::perform() -> int
{
	int running_handles;
	invoke(curl_multi_perform, _handle, &running_handles);
	return running_handles;
}

auto multi_ref::socket_action(socket_t sockfd, int ev_bitmask) -> int
{
	int running_handles;
	invoke(curl_multi_socket_action, _handle, sockfd, ev_bitmask,
	       &running_handles);
	return running_handles;
}

auto multi_ref::info_read() noexcept -> info_read_proxy
{
	return info_read_proxy{_handle};
}

auto multi_ref::wait(std::chrono::milliseconds ms) -> int
{
	int numfds;
	invoke(curl_multi_wait, _handle, nullptr, 0, ms.count(), &numfds);
	return numfds;
}

} // namespace curl
namespace curl { // multi_ref::info_read_proxy

using iterator = multi_ref::info_read_proxy::iterator;
iterator::iterator(CURLM* h) noexcept
: _handle(h)
{
	++*this;
}

auto iterator::operator++() noexcept -> iterator&
{
	_message = curl_multi_info_read(_handle, &_remaining);
	return *this;
}

auto iterator::operator*() noexcept -> multi_ref::info_read_message
{
	return {
		_message->msg,
		easy_ref(_message->easy_handle),
		_message->msg == CURLMSG_DONE ? _message->data.result : CURLE_OK,
		_message->msg == CURLMSG_DONE ? nullptr : _message->data.whatever
	};
}

} // namespace curl
namespace curl { // multi
multi::multi()
{
	init();
}

multi::multi(multi &&x) noexcept
{
	reset(std::exchange(x._handle, nullptr));
}

auto multi::operator=(multi &&x) noexcept -> multi&
{
	reset(std::exchange(x._handle, nullptr));
	return *this;
}

multi::~multi() noexcept
{
	reset();
}
} // namespace curl
