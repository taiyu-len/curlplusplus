#include "curl++/easy.hpp"
#include "curl++/invoke.hpp"
#include "curl++/option.hpp" // for handler
#include <curl/curl.h>       // for curl_easy_cleanup, curl_easy_init
#include <stdexcept>         // for runtime_error
#include <utility>           // for exchange

namespace curl { // easy_ref

void easy_ref::init()
{
	auto tmp = curl_easy_init();
	if (tmp == nullptr)
	{
		throw std::runtime_error("easy_ref failed to initialize");
	}
	reset(tmp);
}

void easy_ref::reset(CURL* h) noexcept
{
	curl_easy_cleanup(std::exchange(_handle, h));
}

void easy_ref::pause(pause_flags flag)
{
	invoke(curl_easy_pause, _handle, static_cast<long>(flag));
}

void easy_ref::perform()
{
	invoke(curl_easy_perform, _handle);
}

} // namespace curl
namespace curl { // easy

easy::easy()
{
	init();
}

easy::easy(easy &&x) noexcept
	: easy_ref(std::exchange(x._handle, nullptr))
{}

auto easy::operator=(easy && x) noexcept -> easy&
{
	reset(std::exchange(x._handle, nullptr));
	return *this;
}

easy::~easy() noexcept
{
	reset();
}

} // namespace curl

