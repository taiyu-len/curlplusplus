#ifndef CURLPLUSPLUS_MULTI_HANDLE_HPP
#define CURLPLUSPLUS_MULTI_HANDLE_HPP
#include "curl++/multi_events.hpp" // for multi_events
#include "curl++/multi_opt.hpp"    // for multi_option
#include "curl++/option.hpp"       // for handler
#include "curl++/types.hpp"        // for code
#include <curl/curl.h>             // for CURLM*
namespace curl {
struct multi_handle
	: public multi_events
{
	/** Construct multi handle.
	 * @throws std::runtime_error on failure to create handle.
	 */
	multi_handle();

	/** Cleanup handle */
	~multi_handle() noexcept;

	/** Transfer ownership over handle. */
	multi_handle(multi_handle &&) noexcept;
	multi_handle& operator=(multi_handle &&) noexcept;

	template<CURLMoption o, typename T>
	auto set(detail::multi_option<o, T> x) noexcept -> curl::code;

	template<class E>
	void set_handler(option::handler<E> x) noexcept;
private:
	CURLM *handle;
};

template<CURLMoption o, typename T>
auto multi_handle::set(detail::multi_option<o, T> x) noexcept -> curl::code
{
	return curl_multi_setopt(handle, o, x.value);
}

template<class E>
void multi_handle::set_handler(option::handler<E> x) noexcept
{
	x.multi(this->handle);
}
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_HANDLE_HPP

