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
 * - seek     * see CURLOPT_SEEKFUNCTION
 * - write    * see CURLOPT_WRITEFUNCTION
 *
 * TODO add more handlers for CURLOPT_*FUNCTION's
 */
template<typename T>
struct easy
: public easy_handle
{
	easy() noexcept;
private:
	auto self() noexcept -> T*;
	using easy_handle::easy_handle;
	using easy_handle::set_handler;
};

template<class T>
easy<T>::easy() noexcept
{
	set_handler< debug,    true >(self());
	set_handler< header,   true >(self());
	set_handler< read,     true >(self());
	set_handler< seek,     true >(self());
	set_handler< write,    true >(self());
	set_handler< progress, true >(self());
}

template<typename T>
auto easy<T>::self() noexcept -> T*
{
	return static_cast<T*>(this);
}

} // namespace curl
#endif // CURLPLUSPLUS_GLOBAL_HPP
