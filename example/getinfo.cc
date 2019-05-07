#include <curl++/easy_handle.hpp>
#include <iostream>
#include <exception>
int main( void ) try
{
	namespace o = curl::option;
	namespace i = curl::info;
	auto curl = curl::easy_handle();
	curl.set(o::url("https://www.example.com"));
	auto r = curl.perform();
	if (! r)
	{
		auto ct = curl.get(i::content_type());
		if (! ct.empty())
		{
			std::cout << "We recieved Content-Type: " << ct << '\n';
		}
	}
	return bool(r);
	
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
