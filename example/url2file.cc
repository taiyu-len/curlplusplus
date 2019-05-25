#include <curl++/easy.hpp>
#include <curl++/global.hpp>
#include <iostream>
#include <fstream>
struct to_file : curl::easy<to_file>
{
	to_file(const char* url, const char* filename);
	size_t handle(write w) noexcept;
private:
	std::ofstream stream;
};

static curl::error_buffer ebuffer;

int main(int argc, char *argv[]) try
{
	if(argc < 2) {
		printf("Usage: %s <URL>\n", argv[0]);
		return 1;
	}
	auto g = curl::global{};
	auto request = to_file{argv[1], "page.out"};
	request.perform();
	return 0;
}
catch (curl::code const& e)
{
	fprintf(stderr, "%s\n", ebuffer.c_str());
}
catch (std::exception const& e)
{
	fprintf(stderr, "%s\n", e.what());
	return 1;
}

to_file::to_file(const char* url, const char* filename)
: stream(filename, std::ios::binary)
{
	namespace o = curl::option;
	set(o::url(url));
	set(o::verbose(true));
	set(o::error_buffer(ebuffer));
}
size_t to_file::handle(write w) noexcept
{
	stream.write(w.data(), w.size());
	return stream ? w.size() : 0;
}
