#ifndef CURLPLUSPLUS_EASY_HANDLE_HPP
#define CURLPLUSPLUS_EASY_HANDLE_HPP
#include "curl++/easy_ref.hpp"
#include "curl++/easy_event.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>
// Contains the light weight wrapper over a curl easy handle

namespace curl {
/** light RAII wrapper around CURL_easy handle.
 *
 */
struct easy_handle : public easy_ref
{
	/** Constructs easy handle.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();

	/** cleanup handle and associated state. */
	~easy_handle() noexcept;

protected:
	/** Transfer ownership over handle. */
	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

	/** Set the event handler. */
	template<class E>
	void set_handler(detail::callback<E>) noexcept;
private:
	using easy_ref::handle;
};

template<class E>
void easy_handle::set_handler(detail::callback<E> x) noexcept
{
	x.set_handler(this->handle);
}
} // namespace curl
#endif // CURLPLUSPLUS_EASY_HANDLE_HPP
