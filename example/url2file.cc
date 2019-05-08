#include <curl++/easy.hpp>
#include <curl++/global.hpp>
#include <iostream>
#include <fstream>
struct to_file : curl::easy<to_file>
{
	to_file(const char* url, const char* filename)
	: stream(filename, std::ios::binary)
	{
		namespace o = curl::option;
		set(o::url(url));
		set(o::verbose(true));
	}
	size_t handle(write w) noexcept
	{
		stream.write(w.data, w.size);
		return stream ? w.size : 0;
	}
private:
	std::ofstream stream;
};

int main(int argc, char *argv[]) try
{
	if(argc < 2) {
		printf("Usage: %s <URL>\n", argv[0]);
		return 1;
	}
	curl::global g;
	to_file request(argv[1], "page.out");
	return bool(request.perform());
}
catch (std::exception const& e)
{
	std::cerr << e.what() << '\n';
	return 1;
}
