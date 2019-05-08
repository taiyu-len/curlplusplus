#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "curl++/easy_handle.hpp" // for easy_handle
#include "curl++/option.hpp"      // for handler
#include "curl++/types.hpp"       // for code
namespace curl {
/** CRTP easy handle that automatically sets callbacks and data.
 * @param T The parent type.
 *
 * callbacks are conditionally set in the constructor based on what overloads
 * of the function `handle` exist in T.
 * each overload handles a different event.
 * - cleanup
 * responsible for managing the lifetime of the object itself, after this
 * is called, the object should be assumed dead.
 *
 * - debug
 * called by libcurl, see CURLOPT_DEBUGFUNCTION
 *
 * - header
 * called by libcurl, see CURLOPT_HEADERFUNCTION
 *
 * - progress
 * called by libcurl, see CURLOPT_XFERINFOFUNCTION
 *
 * - read
 * called by libcurl, see CURLOPT_READFUNCTION
 *
 * - write
 * called by libcurl, see CURLOPT_WRITEFUNCTION
 *
 * TODO add more handlers for CURLOPT_*FUNCTION's
 */
template<typename T>
struct easy
: public easy_handle
{
	easy()
	{
		auto self = static_cast<T*>(this);
		// set event handlers conditionally.
		set_handler(option::handler< cleanup  >(self));
		set_handler(option::handler< debug    >(self));
		set_handler(option::handler< header   >(self));
		set_handler(option::handler< read     >(self));
		set_handler(option::handler< seek     >(self));
		set_handler(option::handler< write    >(self));
		set_handler(option::handler< progress >(self));
	}
	void handle(curl::code) const noexcept {};
private:
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};
} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
