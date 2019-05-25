#ifndef CURLPLUSPLUS_BUFFER_HPP
#define CURLPLUSPLUS_BUFFER_HPP
#include <cstddef>
#include <iterator>
namespace curl {
struct const_buffer {
	using value_type             = char;
	using size_type              = std::size_t;
	using const_iterator         = const char*;
	using const_pointer          = const char*;
	using const_reference        = const char&;

	const_buffer(char* p, size_type s) noexcept : _data(p) , _size(s) {}

	auto operator[](size_type i) const noexcept -> const_reference
	{
		return _data[i];
	}
	auto begin() const noexcept -> const_iterator
	{
		return _data;
	}
	auto end()   const noexcept -> const_iterator
	{
		return _data + _size;
	}

	auto empty() const noexcept -> bool
	{
		return _size == 0;
	}

	auto data()  const noexcept -> const_pointer
	{
		return _data;
	}
	auto size()  const noexcept -> size_type
	{
		return _size;
	}

protected:
	char * _data;
	size_t _size;
};

// mutable buffer
struct buffer : public const_buffer {
	using value_type             = const_buffer::value_type;
	using size_type              = const_buffer::size_type;
	using iterator               = char*;
	using pointer                = char*;
	using reference              = char&;

	using const_buffer::const_buffer;

	using const_buffer::operator[];
	auto operator[](size_type i) noexcept -> reference
	{
		return _data[i];
	}

	using const_buffer::begin;
	auto begin() noexcept -> iterator
	{
		return _data;
	}

	using const_buffer::end;
	auto end() noexcept -> iterator
	{
		return _data + _size;
	}

	using const_buffer::data;
	auto data() noexcept -> pointer
	{
		return _data;
	}
};
} // namespace curl
#endif // CURLPLUSPLUS_BUFFER_HPP
