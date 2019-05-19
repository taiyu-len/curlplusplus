#include <curl++/easy.hpp>
#include <iostream>
#include <exception>
int main(void) try
{
	namespace o = curl::option;
	namespace i = curl::info;
	auto curl = curl::easy_handle();
	curl.set(o::url("https://www.example.com"));
	curl.perform();
	auto ct = curl.get(i::content_type());
	if (! ct.empty())
	{
		std::cout << "We recieved Content-Type: " << ct << '\n';
	}
	return 0;
	
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
