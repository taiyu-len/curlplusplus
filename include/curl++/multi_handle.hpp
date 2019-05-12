#ifndef CURLPLUSPLUS_MULTI_HANDLE_HPP
#define CURLPLUSPLUS_MULTI_HANDLE_HPP
#include "curl++/easy_ref.hpp"     // for easy_ref
#include "curl++/multi_events.hpp" // for multi_events
#include "curl++/multi_opt.hpp"    // for multi_option
#include "curl++/multi_ref.hpp"    // for multi_ref
#include "curl++/option.hpp"       // for handler
#include "curl++/types.hpp"        // for code
#include <curl/curl.h>             // for CURLM*
namespace curl {
struct multi_handle
	: public multi_ref
	, public multi_events
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

	/** Set event handler. */
	template<class E>
	void set_handler(option::handler<E> x) noexcept;
private:
	using multi_ref::handle;
};

template<class E>
void multi_handle::set_handler(option::handler<E> x) noexcept
{
	x.multi(this->handle);
}
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_HANDLE_HPP

