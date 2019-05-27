#ifndef CURLPLUSPLUS_BUFFER_HPP
#define CURLPLUSPLUS_BUFFER_HPP
#include <cstddef>
#include <iterator>
namespace curl {
namespace detail {
/**
 * Buffer of Ts.
 */
template<typename T>
struct buffer {
	using value_type = T;
	using size_type  = std::size_t;
	using iterator   = T*;
	using pointer    = T*;
	using reference  = T&;

	constexpr buffer(T* d, size_type s) noexcept
	: _data(d)
	, _size(s)
	{}

	constexpr auto operator[](size_type i) const noexcept -> reference
	{
		return _data[i];
	}

	constexpr auto begin() const noexcept -> iterator
	{
		return _data;
	}

	constexpr auto end() const noexcept -> iterator
	{
		return _data + _size;
	}

	constexpr auto empty() const noexcept -> bool
	{
		return _size == 0;
	}

	constexpr auto data() const noexcept -> pointer
	{
		return _data;
	}

	constexpr auto size() const noexcept -> size_type
	{
		return _size;
	}

protected:
	T*     _data;
	size_t _size;
};
} // namespace detail

using const_buffer   = detail::buffer<const char>;
using mutable_buffer = detail::buffer<char>;

} // namespace curl
#endif // CURLPLUSPLUS_BUFFER_HPP
