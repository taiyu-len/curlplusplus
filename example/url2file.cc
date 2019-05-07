#include <curl++/easy.hpp>
#include <curl++/global.hpp>
#include <iostream>
#include <cstdlib>
struct to_file : curl::easy<to_file>
{
	to_file(const char* url, const char* filename)
	: stream(fopen(filename, "wb"))
	{
		if (stream == nullptr)
		{
			 throw std::runtime_error("failed to open file for writing");
		}
		namespace o = curl::option;
		set(o::url(url));
		set(o::verbose(true));
	}
	~to_file() noexcept
	{
		fclose(stream);
	}
	size_t handle(write w) noexcept
	{
		auto written = fwrite(w.data, 1, w.size, stream);
		return written;
	}
private:
	FILE* stream;
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
} catch (std::exception const& e) {
	std::cerr << e.what() << '\n';
	return 1;
}
