#ifndef CURLPLUSPLUS_PRIVATE_T_HPP
#define CURLPLUSPLUS_PRIVATE_T_HPP
namespace curl {
/* implements cleanup callback type using private data */
namespace detail {
struct private_t
{
	/** Callback called in easy_t destructor */
	int (*fn)(void*);
	/** pointer passed into fn for cleanup */
	void* ptr;
	/** private pointer set by user */
	void* data;
};
} // namespace detail
} // namespace curl

#endif // CURLPLUSPLUS_PRIVATE_T_HPP

