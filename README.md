an experimental lightweight wrapper library focused on automatically setting
event handlers on construction.

example code
```c++
#include "curl++/easy.hpp"
#include <iostream>

// Prints headers to stdout
struct eh : curl::easy<eh> {
	size_t handle(curl::event::write w) {
		return 0;
	}
	size_t handle(curl::event::header x) {
		std::printf("%*s", x.size, x.data);
		return x.size;
	}
};

int main() {
	eh h;
	h.set(curl::option::url{"www.example.com"});
	h.perform();
}
```
