/*
 * Example source code testing out all features.
 */

#include "curl++/easy.hpp"
#include <iostream>
struct eh : public curl::easy<eh> {
	size_t write(curl::event::write w) {
		return w.pause;
	}
	size_t handle(curl::event::header x) {
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

