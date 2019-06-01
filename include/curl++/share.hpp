#ifndef CURLPLUSPLUS_SHARE_HPP
#define CURLPLUSPLUS_SHARE_HPP
#include <curl/curl.h>
namespace curl {

struct share_ref {
protected:
	CURLSH* _handle = nullptr;
public:
	/**
	 * Construct empty handle by default
	 */
	share_ref() noexcept = default;

	/**
	 * Construct from given raw handle.
	 */
	share_ref(CURLSH *handle) noexcept
	: handle(h)
	{};
};

struct share_handle : share_ref
{
	share_handle();
	~share_handle() noexcept;
};

} // namespace curl
#endif // CURLPLUSPLUS_SHARE_HPP
