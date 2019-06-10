an experimental lightweight wrapper library focused on automatically setting
event handlers on construction.

example code
```c++
#include "curl++/easy.hpp"
#include <iostream>

// Prints headers to stdout
struct eh : curl::easy_base<eh> {
	size_t on(write w) {
		return 0;
	}
	size_t on(header x) {
		std::printf("%*s", x.size, x.data);
		return x.size;
	}
};

int main() try {
	eh h;
	h.url("www.example.com");
	h.perform();
} catch (curl::code &c) {
	if (c == CURLE_WRITE_ERROR)
		return 0;
	std::cout << c.what() << '\n';
}
```

see example dir for more examples.
