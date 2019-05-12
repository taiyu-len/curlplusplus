#ifndef CURLPLUSPLUS_MULTI_HPP
#define CURLPLUSPLUS_MULTI_HPP
#include "curl++/multi_handle.hpp" // for multi_handle
#include "curl++/option.hpp"       // for handler
namespace curl {
/** CRTP multi handle that automatically sets callbacks and data.
 * @param T the parent type.
 */
template<typename T>
struct multi
: public multi_handle
{
	multi() noexcept;
private:
	using multi_handle::multi_handle;
	using multi_handle::set_handler;
};

template<class T>
multi<T>::multi() noexcept
{
	auto self = static_cast<T*>(this);
	// set event handlers conditionally
	set_handler< push   >(self);
	set_handler< timer  >(self);
	set_handler< socket >(self);
}
} // namespace curl
#endif // CURLPLUSPLUS_MULTI_HPP
