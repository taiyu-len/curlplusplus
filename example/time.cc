#include <chrono>
#include <curl++/easy.hpp>
#include <iostream>
struct nowrite {
	static size_t on(curl::easy::write w) noexcept
	{
		return w.size();
	}
};

void print_time(const char* label, std::chrono::microseconds us)
{
	// cast to milliseconds
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(us);
	std::cout << label << ms.count() << " ms\n";
}

int main() try
{
	auto request = curl::easy();

	request.url("https://www.example.com");
	request.set_handler<curl::easy::write, nowrite>();
	request.perform();

	print_time("namelookup time    = ", request.namelookup_time());
	print_time("connect time       = ", request.connect_time());
	print_time("appconnect time    = ", request.appconnect_time());
	print_time("pretransfer time   = ", request.pretransfer_time());
	print_time("starttransfer time = ", request.starttransfer_time());
	print_time("total time         = ", request.total_time());
	print_time("redirect time      = ", request.redirect_time());
	return 0;
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
