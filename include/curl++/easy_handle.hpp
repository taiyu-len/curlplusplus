#ifndef CURLPLUSPLUS_EASY_HANDLE_HPP
#define CURLPLUSPLUS_EASY_HANDLE_HPP
#include "curl++/easy_ref.hpp"
#include "curl++/types.hpp"
#include <curl/curl.h>
// Contains the light weight wrapper over a curl easy handle

namespace curl {
/** light RAII wrapper around CURL_easy handle.
 *
 */
struct easy_handle : public easy_ref
{
	/** Constructs easy handle.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();

	/** cleanup handle and associated state. */
	~easy_handle() noexcept;

protected:
	/** Transfer ownership over handle. */
	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

	//@{
	/** Sets both the callback and data options safely. */
	// curl does not support a cleanup function, so emulate it through the
	// private pointer instead.
	template<typename T>
	void cleanup_function(cleanup_cb<T>*, T*) noexcept;
	void cleanup_function(nullptr_t) noexcept;

	// requires verbose setting to be true
	template<typename T>
	void debug_function(debug_cb<T>*, T*) noexcept;
	void debug_function(nullptr_t) noexcept;

	template<typename T>
	void header_function(header_cb<T>*, T*) noexcept;
	void header_function(nullptr_t) noexcept;

	template<typename T>
	void read_function(read_cb<T>*, T*) noexcept;
	void read_function(nullptr_t) noexcept;

	template<typename T>
	void seek_function(seek_cb<T>*, T*) noexcept;
	void seek_function(nullptr_t) noexcept;

	template<typename T>
	void write_function(write_cb<T>*, T*) noexcept;
	void write_function(nullptr_t) noexcept;

	// requires noprogress to be false to call these
	// TODO: consider whether to set progress here or not
	template<typename T>
	void xferinfo_function(xferinfo_cb<T>*, T*) noexcept;
	void xferinfo_function(nullptr_t) noexcept;
	//@}
private:
	// type unsafe versions
	void cleanup_function(cleanup_cb<void>*, void*) noexcept;
	void debug_function(debug_cb<void>*, void*) noexcept;
	void header_function(header_cb<void>*, void*) noexcept;
	void read_function(read_cb<void>*, void*) noexcept;
	void seek_function(seek_cb<void>*, void*) noexcept;
	void write_function(write_cb<void>*, void*) noexcept;
	void xferinfo_function(xferinfo_cb<void>*, void*) noexcept;
};

#define DEFINE_EASY_CALLBACK_FN(NAME) \
template<typename T> \
void easy_handle::NAME ##_function(NAME ##_cb<T>* fn, T* data) noexcept \
{ \
	NAME ##_function(reinterpret_cast<NAME ##_cb<void>*>(fn), \
	                 static_cast<void*>(data)); \
}
DEFINE_EASY_CALLBACK_FN(cleanup);
DEFINE_EASY_CALLBACK_FN(debug);
DEFINE_EASY_CALLBACK_FN(header);
DEFINE_EASY_CALLBACK_FN(read);
DEFINE_EASY_CALLBACK_FN(seek);
DEFINE_EASY_CALLBACK_FN(write);
DEFINE_EASY_CALLBACK_FN(xferinfo);
#undef DEFINE_EASY_CALLBACK_FN

} // namespace curl
#endif // CURLPLUSPLUS_EASY_HANDLE_HPP
