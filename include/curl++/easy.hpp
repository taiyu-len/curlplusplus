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
 * - debug    * see CURLOPT_DEBUGFUNCTION
 * - header   * see CURLOPT_HEADERFUNCTION
 * - progress * see CURLOPT_XFERINFOFUNCTION
 * - read     * see CURLOPT_READFUNCTION
 * - write    * see CURLOPT_WRITEFUNCTION
 *
 * TODO add more handlers for CURLOPT_*FUNCTION's
 */
template<typename T>
struct easy
: public easy_handle
{
	easy() noexcept;
	void handle(curl::code) const noexcept {};
private:
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};

template<class T>
easy<T>::easy() noexcept
{
	auto self = static_cast<T*>(this);
	// set event handlers conditionally.
	set_handler< debug    >(self);
	set_handler< header   >(self);
	set_handler< read     >(self);
	set_handler< seek     >(self);
	set_handler< write    >(self);
	set_handler< progress >(self);
}

} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
