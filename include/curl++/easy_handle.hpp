#ifndef CURLPLUSPLUS_EASY_HANDLE_HPP
#define CURLPLUSPLUS_EASY_HANDLE_HPP
#include "curl++/easy_ref.hpp"    // for_easy_ref, easy_ref::handle
namespace curl {
/**
 * Lightweight RAII class for a curl easy handle.
 */
struct easy_handle
	: public easy_ref
{
	/** Constructs easy handle.
	 * @throws std::runtime_error on failure to create handle.
	 */
	easy_handle();

	/** cleanup handle. */
	~easy_handle() noexcept;

	/** Transfer ownership over handle. */
	easy_handle(easy_handle &&) noexcept;
	easy_handle& operator=(easy_handle &&) noexcept;

private:
	using easy_ref::handle;
};

} // namespace curl
#endif // CURLPLUSPLUS_EASY_HANDLE_HPP
