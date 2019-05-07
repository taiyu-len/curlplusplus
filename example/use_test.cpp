/*
 * Example source code testing out all features.
 */

#include "curl++/easy.hpp"
#include <iostream>
struct eh : public curl::easy<eh> {
	size_t handle(write w) {
		return 0;
	}
	size_t handle(header x) {
		return x.size;
	}
};

int main() {
	namespace o = curl::option;
	eh h;
	h.set(o::url{"www.example.com"});
	h.set(o::verbose{true});
	std::cout << h.perform().what() << '\n';
}

