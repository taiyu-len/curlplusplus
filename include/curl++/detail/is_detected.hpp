#ifndef CURLPLUSPLUS_DETAIL_IS_DETECTED_HPP
#define CURLPLUSPLUS_DETAIL_IS_DETECTED_HPP
#include <type_traits>
namespace curl {
namespace detail {

template<typename... Ts> struct make_void { using type = void; };
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

template<class Void, template<class...> class Op, class... Args>
struct detector : std::false_type
{
	using type = void;
};

template<template<class...> class Op, class... Args>
struct detector<void_t<Op<Args...>>, Op, Args...>
	: std::true_type
{
	using type = Op<Args...>;
};

template<template<class...> class Op, class... Args>
using is_detected = detector<void, Op, Args...>;

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_DETAIL_IS_DETECTED_HPP
