#ifndef CURLPLUSPLUS_SHARE_HPP
#define CURLPLUSPLUS_SHARE_HPP
#include <curl/curl.h>
namespace curl { // share_ref

struct share_ref {
protected:
	CURLSH* handle = nullptr;
public:
	share_ref() noexcept = default;
	share_ref(CURLSH *h) noexcept : handle(h) {};
};

} // namespace curl
namespace curl { // share_handle

struct share_handle : share_ref
{
	share_handle();
	~share_handle() noexcept;
};

} // namespace curl
#endif // CURLPLUSPLUS_SHARE_HPP
