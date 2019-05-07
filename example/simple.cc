#include <curl++/easy.hpp>
#include <iostream>
#include <exception>
int main() try
{
	namespace o = curl::option;
	auto curl = curl::easy_handle();
	curl.set(o::url("https://example.com"));
	curl.set(o::follow_location(true));
	auto r = curl.perform();
	if (r)
	{
		std::cerr << "perform() failed: " << r.what() << '\n';
	}
	return bool(r);
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
