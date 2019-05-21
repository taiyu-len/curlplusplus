#include <curl++/easy.hpp>
#include <iostream>
#include <chrono>
struct nowrite {
	static size_t handle(curl::easy_ref::write w) noexcept
	{
		return w.size;
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
	namespace o = curl::option;
	namespace i = curl::info;
	namespace ev = curl::easy_events;
	auto request = curl::easy_handle();

	request.set(o::url("https://www.example.com"));
	request.set_handler<ev::write, nowrite>();
	request.perform();

	print_time("namelookup time    = ", request.get(i::namelookup_time));
	print_time("connect time       = ", request.get(i::connect_time));
	print_time("appconnect time    = ", request.get(i::appconnect_time));
	print_time("pretransfer time   = ", request.get(i::pretransfer_time));
	print_time("starttransfer time = ", request.get(i::starttransfer_time));
	print_time("total time         = ", request.get(i::total_time));
	print_time("redirect time      = ", request.get(i::redirect_time));
	return 0;
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
