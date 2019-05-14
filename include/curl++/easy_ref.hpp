#ifndef CURLPLUSPLUS_EASY_REF_HPP
#define CURLPLUSPLUS_EASY_REF_HPP
#include "curl++/easy_info.hpp"
#include "curl++/option.hpp"    // for handler
#include "curl++/easy_opt.hpp"  // for easy_option
#include "curl++/types.hpp"     // for pause
#include <curl/curl.h>          // for CURL*

namespace curl {
/** Lightweight non-owning reference to a curl easy handle.
 * Provides all desired functionality for modifying and querying the underlying
 * curl handle.
 */
struct easy_ref
{
	/** construct null reference by default. */
	easy_ref() noexcept = default;

	/** construct from a raw CURL handle type */
	easy_ref(CURL *handle) noexcept;

	/** Check whether this type is valid or not.
	 * @return true if handle != nullptr, else false.
	 */
	explicit operator bool() const noexcept;

	/** Pause or resume receiving or sending data.
	 * @param flag whether to pause or resume transfer
	 * @throws curl::code
	 * @pre *this
	 * see curl_easy_pause
	 */
	void pause(curl::pause flag);

	/** Perform the request.
	 * @throws curl::code
	 * @pre *this
	 */
	void perform();

	/** Set Easy handle options.
	 * @param option curl::option::* type
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code easy.setopt(o::url{"www.example.com"}); @endcode
	 */
	template<CURLoption o, typename T>
	void set(detail::easy_option<o, T>);

	/** Get request info.
	 * @param option curl::info::* type
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code auto url = easy.getinfo(i::url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	auto get(info::info<i, T>) const -> T;

	// Events that can emitted by CURL
	struct debug;
	struct header;
	struct read;
	struct seek;
	struct write;
	struct progress;

	/** Set data and event handler.
	 * @param E event that is being handled.
	 * @param S whether to produce a compile error or use null.
	 * @param x object that determines function and data pointer on
	 * construction.
	 * @pre *this
	 * @requires x->handle(E);
	 *
	 * sets the CURLOPT_*FUNCTION and *DATA settings in one go.
	 * if !S produce a compile error if x cannot handle event E.
	 * otherwise set FUNCTION and DATA to noop instead.
	 * (null for everything but for write, in that case use a nowrite
	 *  function)
	 *
	 * example: @code x.set_handler<write>(foo); @endcode
	 */
	template<typename E, bool S = false, typename T>
	void set_handler(T*) noexcept;

	template<typename E>
	void set_handler(option::handler<E> x) noexcept;
protected:
	CURL* handle = nullptr;
};

template<CURLoption o, typename T>
void easy_ref::set(detail::easy_option<o, T> x)
{
	curl::code ec = curl_easy_setopt(handle, o, x.value);
	if (ec) throw ec;
}

template<CURLINFO i, typename T>
auto easy_ref::get(info::info<i, T> x) const -> T
{
	typename info::info<i, T>::value_type y;
	curl::code ec = curl_easy_getinfo(handle, i, &y);
	if (ec) throw ec;
	return x(y);
}

template<typename E, bool S, typename T>
void easy_ref::set_handler(T* x) noexcept
{
	option::handler<E>::template from<S>(x).easy(handle);
}

template<typename E>
void easy_ref::set_handler(option::handler<E> x) noexcept
{
	x.easy(handle);
}
} // namespace curl

// defines easy_ref event types.
// required to go here to avoid circular dependency issue.
#include "curl++/easy_events.hpp"
#endif // CURLPLUSPLUS_EASY_REF_HPP
