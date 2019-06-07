#include <curl++/easy.hpp>
#include <curl++/share.hpp>
#include <iostream>

struct my_share : curl::share_base<my_share> {
	my_share() {
		share_data(connect);
	}
	void on(lock) {
		std::cout << "Mutex locked\n";
	}
	void on(unlock) {
		std::cout << "Mutex unlocked\n";
	}
};

int main()
{
	my_share share;

	int i;

	/* Loop the transfer and cleanup the handle properly every lap. This
	 * will still reuse connections since the pool is in the shared object!
	 */

	for(i = 0; i < 3; i++) try {
		curl::easy req;
		req.url("https://www.example.com");
		req.share(share);
		req.perform();
	} catch (curl::code const& c) {
		std::cerr << c.what() << '\n';
	}
	return 0;
}
