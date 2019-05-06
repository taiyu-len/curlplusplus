/*
 * Example source code testing out all features.
 */

#include "curl++/easy.hpp"
#include <iostream>
struct eh : curl::easy<eh> {
	size_t handle(curl::event::write x) {
		std::cout << "=> Write Recieved " << x.size << "bytes\n";
		return x.size;
	}
	size_t handle(curl::event::header x) {
		std::cout << "=> Header Recieved " << x.size << "bytes\n";
		std::printf("%*s", x.size, x.data);
		return x.size;
	}
};

int main() {
	namespace o = curl::option;
	namespace i = curl::info;
	eh h;
	h.set(o::url{"www.example.com"});
	h.perform();
}

