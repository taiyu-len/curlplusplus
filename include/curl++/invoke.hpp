#ifndef CURLPLUSPLUS_INVOKE_HPP
#define CURLPLUSPLUS_INVOKE_HPP
#include "types.hpp"
namespace curl {
/**
 * Utility function to wrap curl function calls to handle errors appropriatly.
 */
template<typename Fn, typename... Args>
void invoke(Fn&& fn, Args&&... args)
{
	auto c_errcode = std::forward<Fn>(fn)(std::forward<Args>(args)...);
	auto errcode = to_code(c_errcode);
	if (errcode)
	{
		throw errcode;
	}
}

/**
 * Utility function that returns a value as the last parameter.
 */
template<typename R, typename Fn, typename... Args>
auto invoke_r(Fn&& fn, Args&&...args) -> R
{
	R x;
	invoke(std::forward<Fn>(fn), std::forward<Args>(args)..., &x);
	return x;
}

} // namespace curl
#endif // CURLPLUSPLUS_INVOKE_HPP
