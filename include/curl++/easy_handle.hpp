#ifndef CURLPLUSPLUS_EASY_HANDLE_HPP
#define CURLPLUSPLUS_EASY_HANDLE_HPP
#include "curl++/easy_events.hpp"
#include "curl++/easy_ref.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>
namespace curl {
/** Lightweight RAII class for a curl easy handle.
 * Allows all functionality that easy_ref does, and allows setting of
 * event handlers.
 * Makes event types that can be handled visible from easy_handle scope.
 */
struct easy_handle
	: public easy_ref
	, public easy_events
{
	/** Constructs easy handle.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();

	/** cleanup handle and associated state. */
	~easy_handle() noexcept;

	/** Transfer ownership over handle. */
	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

	/** Set event handler for a given event */
	template<typename E>
	void set_handler(option::handler<E> x) noexcept;
private:
	using easy_ref::handle;
};

template<class E>
void easy_handle::set_handler(option::handler<E> x) noexcept
{
	x.easy(this->handle);
}
} // namespace curl
#endif // CURLPLUSPLUS_EASY_HANDLE_HPP
