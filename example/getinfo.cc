#include <curl++/easy.hpp>
#include <exception>
#include <iostream>
int main() try
{
	curl::easy curl;
	curl.url("https://www.example.com");
	curl.perform();
	auto ct = curl.content_type();
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
