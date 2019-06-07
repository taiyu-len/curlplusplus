#include <curl++/easy.hpp>
#include <exception>
#include <iostream>
int main() try
{
	auto curl = curl::easy();
	curl.url("https://example.com");
	curl.follow_location(true);
	curl.perform();
	return 0;
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
