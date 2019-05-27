/* Example source code testing out different use cases.
 */
#include <curl++/easy.hpp>
#include <iostream>
struct print_callbacks : curl::easy_base<print_callbacks> {
	print_callbacks() {
		namespace o = curl::option;
		set(o::url("www.example.com"));
		set(o::verbose(true));
		set(o::no_progress(false));
	}
	static size_t handle(write w) {
		std::cout << "called write   : received " << w.size() << '\n';
		return w.size();
	}
	static size_t handle(header w) {
		std::cout << "called header  : recieved " << w.size() << '\n';
		printf("    %*s", int(w.size()), w.data());
		return w.size();
	}
	static size_t handle(read w) {
		std::cout << "called read    : " << w.size() << '\n';
		return w.size();
	}
	static int handle(seek) {
		std::cout << "called seek    :\n";
		return CURL_SEEKFUNC_OK;
	}
	static int handle(debug) {
		std::cout << "called debug   :\n";
		return 0;
	}
	static int handle(progress p) {
		std::cout << "called progress:\n";
		std::cout << "    dltotal = " << p.dltotal;
		std::cout << "    dlnow = " << p.dlnow << '\n';
		std::cout << "    ultotal = " << p.ultotal;
		std::cout << "    ulnow = " << p.ulnow << '\n';
		return 0;
	}
};

// plain easy case
void plain_usage()
{
	namespace o = curl::option;
	// create a easy handle.
	auto h = curl::easy();
	// setup options
	h.set(o::url("https://www.example.com"));
	h.set_handler<curl::easy::header, print_callbacks>();
	h.set_handler<curl::easy::write, print_callbacks>();
	h.perform();
}

int main() try {
	std::cout << "==\n== Plain use case\n==\n";
	plain_usage();
	std::cout << "==\n== crtp use case\n==\n";
	print_callbacks().perform();
	return 0;
} catch (std::exception const& e) {
	std::cerr << e.what() << '\n';
	return 1;
}

