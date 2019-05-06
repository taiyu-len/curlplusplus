#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "curl++/easy_handle.hpp"
#include <type_traits>
#include <curl/curl.h>
namespace curl {
/** CRTP easy handle that automatically sets callbacks and data.
 * callbacks are set in constructor based on what overloads of the handle
 * function exists in the parent type.
 *
 * each overload handles a different event.
 * - cleanup_event:  occurs when an easy request is completed to cleanup state.
 * - debug_event:    CURLOPT_DEBUGFUNCTION
 * - *code:          occurs when a curl function returns an error code.
 * - header_event:   CURLOPT_HEADERFUNCTION
 * - progress_event: CURLOPT_XFERINFOFUNCTION
 * - read_event:     CURLOPT_READFUNCTION
 * - write_event:    CURLOPT_WRITEFUNCTION
 *
 * static versions of these callbacks allow the user to set the DATA pointer for
 * that callback if the type matches.
 * multiple static versions can exist and one can set the callback by setting
 * the data.
 *
 * example usage.
 * \code{.cpp}
struct printer : curl::easy<printer> {
	// the function that will be used for write_function callback
	size_t write(char* data, size_t length) noexcept {
		// print data received
	}
	int cleanup() noexcept {
		// print footer after data received.
	}
};
\endcode
 *
 * There are multiple designs for handling the setting of callbacks.
 * 1. init functions
 * curl::easy will call an init_function for each callback, which will at
 * compile time determine whether to set it or not.
 * - requires function specialization.
 * - clutters private functions in easy type
 *
 * 2. crtp base class
 * curl::easy inherits from empty base classes that take T as template
 * parameter, and a T* as constructor.
 * the type is specialized to set the callback or not depending on T
 * - struct specialization
 * - requires friend of easy
 *
 * 3. argument type specialization.
 * the set handler functions take a structure that sets the function pointer to
 * the right one on construction, or nullptr if there is none.
 * - reduces the callback settings templates in easy_handle to one template
 * - decoupled from other types into its own thing.
 * - simplifies easy constructor.
 *
 *   this looks best so far
 *
 */
template<typename T>
struct easy
	: public easy_handle
{
	easy() {
		auto self = static_cast<T*>(this);
		// set event handlers conditionally.
		set_handler(detail::callback< event::cleanup  >(self));
		set_handler(detail::callback< event::debug    >(self));
		set_handler(detail::callback< event::header   >(self));
		set_handler(detail::callback< event::read     >(self));
		set_handler(detail::callback< event::seek     >(self));
		set_handler(detail::callback< event::write    >(self));
		set_handler(detail::callback< event::progress >(self));
	}
	void handle(curl::code) const noexcept {};
private:
	using easy_handle::set_handler;
};

} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
