#include <curl++/easy.hpp>
#include <curl++/global.hpp>
#include <iostream>
#include <string>
struct print_debug : curl::easy<print_debug>
{
	print_debug(std::string url, bool trace_ascii);
	int handle(debug x) noexcept;
private:
	void dump(const char* text, FILE*, unsigned char*, size_t);
	bool no_hex;
};

static curl::error_buffer ebuffer;
int main(void) try
{
	auto g = curl::global{};
	auto request = print_debug{"https://example.com", true};
	request.perform();
	return 0;
}
catch (std::exception const& e)
{
	fprintf(stderr, "%s: %s\n", e.what(), ebuffer.c_str());
	return 1;
}

print_debug::print_debug(std::string url, bool trace_ascii)
: no_hex(trace_ascii)
{
	namespace o = curl::option;
	/* the DEBUGFUNCTION has no effect until we enable VERBOSE */
	set(o::verbose(true));
	/* example.com is redirected, so we tell libcurl to follow redirection */
	set(o::follow_location(true));
	set(o::url(url));
	set(o::error_buffer(ebuffer));
}

int print_debug::handle(debug x) noexcept
{
	const char *text;
	switch(x.type)
	{
	case CURLINFO_TEXT:
		fprintf(stderr, "== Info: %*s", int(x.size()), x.data());
		/* FALLTHROUGH */ 
	default: /* in case a new one is introduced to shock us */ 
		return 0;
	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		break;
	case CURLINFO_SSL_DATA_OUT:
		text = "=> Send SSL data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		break;
	case CURLINFO_SSL_DATA_IN:
		text = "<= Recv SSL data";
		break;
	}

	dump(text, stderr, (unsigned char*)(x.data()), x.size());
	return 0;
}

void print_debug::dump(const char* text, FILE* stream, unsigned char* ptr, size_t size)
{
	// fit more on screen if we only show ascii
	const auto width = no_hex ? 0x40ul : 0x10ul;
	const auto is_0D0A = [&](size_t i)
	{
		return no_hex && (i+1 < size)
		    && ptr[i] == 0x0D && ptr[i+1] == 0x0A;
	};
	fprintf(stream, "%s, %10.10zu bytes (0x%8.8zx)\n", text, size, size);
	for (size_t i = 0; i < size; i += width)
	{
		fprintf(stream, "%4.4zx: ", i);
		if (! no_hex)
		{
			/* hex not disabled, show it */
			for (size_t c = 0; c < width; ++c)
			{
				if (i + c < size)
				{
					fprintf(stream, "%02x", ptr[i+c]);
				}
				else
				{
					fprintf(stream, "  ");
				}
			}
		}
		for (size_t c = 0; (c < width) && (i + c < size); ++c)
		{
			/* check for 0D0A; if found,
			 * skip past and start a new line of output */
			if (is_0D0A(i+c))
			{
				i += c + 2 - width;
				break;
			}
			auto x = ptr[i+c];
			fprintf(stream, "%c", (x >= 0x20 && x < 0x80) ? x : '.');
			/* check again for 0D0A, to avoid an extra \n if it's at
			 * width */
			if (is_0D0A(i+c))
			{
				i += c + 3 - width;
				break;
			}
		}
		fprintf(stream, "\n");
	}
	fflush(stream);
}
