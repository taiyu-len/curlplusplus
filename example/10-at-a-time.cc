#include "curl++/easy.hpp"
#include "curl++/multi.hpp"
#include "curl++/global.hpp"
#include <vector>
#include <iostream>
#include <chrono>

const extern std::vector<const char*> urls;

struct nowrite {
	static size_t handle(curl::easy_events::write w) {
		return w.size();
	}
};

static void add_transfer(curl::multi_ref cm, const char* url)
{
	curl::easy_handle eh;
	namespace o = curl::option;
	fprintf(stderr, "Adding url %s\n", url);
	eh.set(o::url(url));
	eh.set(o::userdata(url));
	eh.set_handler<curl::easy_events::write, nowrite>();
	/* Could be done on clang with c++17. */
	// eh::set_handle([](eh::write w){ return w.size(); });

	cm.add_handle(eh.release());
}

#define MAX_PARALLEL 10

int main(void) {
	auto g = curl::global();
	auto m = curl::multi_handle();
	namespace o = curl::option;

	m.set(o::max_connects{MAX_PARALLEL});
	auto transfers = size_t(0);
	// add initial transfers
	for (auto const& url : urls)
	{
		add_transfer(m, url);
		if (++transfers == MAX_PARALLEL)
		{
			break;
		}
	}

	do {
		auto active = m.perform();
		for (auto &msg : m.info_read())
		{
			if (msg.msg == CURLMSG_DONE)
			{
				curl::easy_handle eh(msg.easy_handle);
				curl::code        cc(msg.data.result);
				namespace i = curl::info;
				fprintf(stderr, "R: %d - %s < %s >\n",
					cc.value, cc.what(),
					eh.get(i::userdata<const char*>));
				m.remove_handle(eh);
			}
			else
			{
				fprintf(stderr, "E: CURLMsg (%d)\n", msg.msg);
			}
			if (transfers < urls.size()) {
				add_transfer(m, urls[transfers++]);
			}
		}
		if (active > 0)
		{
			using namespace std::chrono_literals;
			m.wait(1s);
		}
		else if (transfers >= urls.size())
		{
			break;
		}
	} while (true);
}

const std::vector<const char*> urls =
{
	"https://www.microsoft.com",
	"https://opensource.org",
	"https://www.google.com",
	"https://www.yahoo.com",
	"https://www.ibm.com",
	"https://www.mysql.com",
	"https://www.oracle.com",
	"https://www.ripe.net",
	"https://www.iana.org",
	"https://www.amazon.com",
	"https://www.netcraft.com",
	"https://www.heise.de",
	"https://www.chip.de",
	"https://www.ca.com",
	"https://www.cnet.com",
	"https://www.mozilla.org",
	"https://www.cnn.com",
	"https://www.wikipedia.org",
	"https://www.dell.com",
	"https://www.hp.com",
	"https://www.cert.org",
	"https://www.mit.edu",
	"https://www.nist.gov",
	"https://www.ebay.com",
	"https://www.playstation.com",
	"https://www.uefa.com",
	"https://www.ieee.org",
	"https://www.apple.com",
	"https://www.symantec.com",
	"https://www.zdnet.com",
	"https://www.fujitsu.com/global/",
	"https://www.supermicro.com",
	"https://www.hotmail.com",
	"https://www.ietf.org",
	"https://www.bbc.co.uk",
	"https://news.google.com",
	"https://www.foxnews.com",
	"https://www.msn.com",
	"https://www.wired.com",
	"https://www.sky.com",
	"https://www.usatoday.com",
	"https://www.cbs.com",
	"https://www.nbc.com/",
	"https://slashdot.org",
	"https://www.informationweek.com",
	"https://apache.org",
	"https://www.un.org",
};
