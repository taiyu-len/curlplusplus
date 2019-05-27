#include "curl++/easy.hpp"
#include "curl++/multi.hpp"
#include "curl++/global.hpp"
#include <vector>
#include <iostream>
#include <chrono>

const extern std::vector<const char*> urls;

// handler object for write events. doesnt do anything.
// could replace with
//   handle.set_handler([](write w) { return w.size(); });
// if a gcc bug was fixed and were using c++17
struct nowrite {
	static size_t handle(curl::easy::write w) {
		return w.size();
	}
};

// add a transfer to a multi handle using an existing easy handle.
static void add_transfer(curl::multi_ref cm, curl::easy_ref ce, const char* url)
{
	namespace o = curl::option;
	fprintf(stderr, "Adding url %s\n", url);
	ce.set(o::url(url));
	ce.set(o::userdata(url));
	ce.set_handler<curl::easy::write, nowrite>();
	cm.add_handle(ce);
}

// add a transfer to a multi handle by creating a new easy handle.
static void add_transfer(curl::multi_ref cm, const char* url)
{
	curl::easy_ref er;
	er.init();
	add_transfer(cm, er, url);
}

#define MAX_PARALLEL 10

int main(void) {
	// initialize global and multi handle.
	auto g = curl::global();
	auto m = curl::multi();
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
		// perform transfers, return active connections
		auto active = m.perform();
		// watch for complete requests
		for (auto msg : m.info_read())
		{
			auto er = msg.ref;
			// print message when done and remove handle.
			if (msg.msg == CURLMSG_DONE)
			{
				auto cc = msg.result;
				namespace i = curl::info;
				fprintf(stderr, "R: %d - %s < %s >\n",
					cc.value, cc.what(),
					er.get(i::userdata<const char*>));
				m.remove_handle(er);
			}
			else
			{
				fprintf(stderr, "E: CURLMsg (%d)\n", msg.msg);
			}
			if (transfers < urls.size()) {
				// reuse handle for new transfer
				add_transfer(m, er, urls[transfers++]);
			} else {
				// or cleanup handle if no more transfers
				er.reset();
			}
		}
		// if we have transfers left wait 1 second or until there is
		// activity on sockets.
		if (active > 0)
		{
			using namespace std::chrono_literals;
			m.wait(1s);
		}
		// otherwise if we do not have any transfers left to make break.
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
