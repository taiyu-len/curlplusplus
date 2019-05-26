#include "curl++/share.hpp"
namespace curl {

share_handle::share_handle() : share_ref(curl_share_init()) {}
share_handle::~share_handle()
{
	curl_share_cleanup(handle);
}

} // namespace curl
