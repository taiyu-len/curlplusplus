#ifndef CURLPLUSPLUS_BUFFER_HPP
#define CURLPLUSPLUS_BUFFER_HPP
#include <cstddef>
namespace curl {
// dumb buffer type. TODO improve it
struct buffer {
	char* data;
	size_t size;
};
} // namespace curl
#endif // CURLPLUSPLUS_BUFFER_HPP
