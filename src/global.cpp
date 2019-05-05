#include <curl++/global.hpp>
#include <stdexcept>
namespace curl {
global::global(flags f)
{
	auto err = curl_global_init(f);
	if (err)
	{
		// TODO use better method to report failure
		throw std::runtime_error(curl_easy_strerror(err));
	}
}

global::global()
: global(curl::global::DEFAULT)
{
	// NOOP
};

global::~global() noexcept
{
	curl_global_cleanup();
}
} // namespace curl
