/* Example source code testing out different use cases.
 */
#include <curl++/easy.hpp>
#include <iostream>
struct print_callbacks : curl::easy_base<print_callbacks> {
	print_callbacks() {
		std::cout << "==\n== crtp use case\n==\n";
		namespace o = curl::option;
		set(o::url("www.example.com"));
		set(o::verbose(true));
		set(o::no_progress(false));
	}
	static size_t on(write w) {
		std::cout << "called write   : received " << w.size() << '\n';
		return w.size();
	}
	static size_t on(header w) {
		std::cout << "called header  : recieved " << w.size() << '\n';
		printf("    %*s", int(w.size()), w.data());
		return w.size();
	}
	static size_t on(read w) {
		std::cout << "called read    : " << w.size() << '\n';
		return w.size();
	}
	static int on(seek) {
		std::cout << "called seek    :\n";
		return CURL_SEEKFUNC_OK;
	}
	static int on(debug) {
		std::cout << "called debug   :\n";
		return 0;
	}
	static int on(progress p) {
		std::cout << "called progress:\n";
		std::cout << "    dltotal = " << p.dltotal;
		std::cout << "    dlnow = " << p.dlnow << '\n';
		std::cout << "    ultotal = " << p.ultotal;
		std::cout << "    ulnow = " << p.ulnow << '\n';
		return 0;
	}
};

struct mem_fn_cb {
	size_t on(curl::easy::write w) {
		std::cout << "member fn| write   : received " << w.size() << '\n';
		std::cout << "  this=" << this << '\n';
		return w.size();
	}
};
struct static_fn_cb {
	static size_t on(curl::easy::write w) {
		std::cout << "static fn| write   : received " << w.size() << '\n';
		return w.size();
	}
};
struct static_fn_with_ptr_cb {
	static size_t on(curl::easy::write w, int *data) {
		std::cout << "data   fn| write   : received " << w.size();
		std::cout << "  data[" << data <<  "] = " << *data << '\n';
		return w.size();
	}
};
struct static_fn_with_value_cb {
	static size_t on(curl::easy::write w, int data) {
		std::cout << "data   fn| write   : received " << w.size();
		std::cout << "  data=" << data << '\n';
		return w.size();
	}
};


// plain case manually testing various kinds of handlers
void plain_usage()
{
	std::cout << "==\n== Plain use case\n==\n";
	namespace o = curl::option;
	// create a easy handle.
	auto h = curl::easy();
	// setup options
	h.set(o::url("https://www.example.com"));

	// callback with member function.
	mem_fn_cb mfc;
	h.set_handler<curl::easy::write>(&mfc);
	h.perform();

	// callback with static member function
	h.set_handler<curl::easy::write, static_fn_cb>();
	h.perform();

	// callback with ptr argument
	int data = 99;
	h.set_handler<curl::easy::write, static_fn_with_ptr_cb>(&data);
	h.perform();

	// callback with value argument
	h.set_handler<curl::easy::write, static_fn_with_value_cb>(&data);
	h.perform();

	// callback with function ptr
	h.set_handler<&static_fn_cb::on>();
	h.perform();

	h.set_handler<&static_fn_with_ptr_cb::on>(&data);
	h.perform();

	h.set_handler<&static_fn_with_value_cb::on>(&data);
	h.perform();
}

int main() try {
	plain_usage();
	print_callbacks().perform();
	return 0;
} catch (std::exception const& e) {
	std::cerr << e.what() << '\n';
	return 1;
}

