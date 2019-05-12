#include <curl++/easy_handle.hpp>
#include <iostream>
#include <exception>
int main() try
{
	namespace o = curl::option;
	auto curl = curl::easy_handle();
	curl.set(o::url("https://example.com"));
	curl.set(o::follow_location(true));
	curl.perform();
	return 0;
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
