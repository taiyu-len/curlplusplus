#include "curl++/multi_ref.hpp"
namespace curl {
multi_ref::multi_ref(CURLM *handle)
: handle(handle)
{
	// NOOP
}

} // namespace curl
