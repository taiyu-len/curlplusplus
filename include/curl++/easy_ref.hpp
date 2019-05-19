#ifndef CURLPLUSPLUS_EASY_REF_HPP
#define CURLPLUSPLUS_EASY_REF_HPP
#include "curl++/invoke.hpp"
#include "curl++/easy_info.hpp"
#include "curl++/easy_opt.hpp"  // for easy_option
#include "curl++/option.hpp"    // for handler
#include "curl++/types.hpp"     // for pause
#include <curl/curl.h>          // for CURL*

namespace curl {
struct multi_ref;
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
	inline void set(detail::easy_option<o, T>);

	/** Get request info.
	 * @param option curl::info::* type
	 * @throws curl::code
	 * @pre *this
	 *
	 * example: @code auto url = easy.getinfo(i::url{}); @endcode
	 */
	template<CURLINFO i, typename T>
	inline auto get(info::info<i, T>) const -> T;

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
	inline void set_handler(T *x) noexcept;

	/** Set data and event handler
	 * @param E event to be handled.
	 * @param T type containing static handle functions for event.
	 * @pre *this
	 *
	 * usage: set_handler<write, T>();
	 */
	template<typename E, typename T>
	inline void set_handler() noexcept;

	/** Set data and event handler
	 * @param E event to be handled.
	 * @param T type containing static handle functions for event.
	 * @param D Data pointer for event
	 * @pre *this
	 *
	 * usage: set_handler<write, T>(d*);
	 */
	template<typename E, typename T, typename D>
	inline void set_handler(D *) noexcept;
protected:
	friend multi_ref;
	CURL* handle = nullptr;
};

template<CURLoption o, typename T>
void easy_ref::set(detail::easy_option<o, T> x)
{
	invoke(curl_easy_setopt, handle, o, x.value);
}

template<CURLINFO i, typename T>
auto easy_ref::get(info::info<i, T> x) const -> T
{
	typename info::info<i, T>::value_type y;
	invoke(curl_easy_getinfo, handle, i, &y);
	return x(y);
}

template<typename E, bool S, typename T>
void easy_ref::set_handler(T* x) noexcept
{
	constexpr auto h = option::handler<E>::template from_mem_fn<T>();
	static_assert(S || bool(h), "Could not extract `t.handle(e)`");
	h.easy(handle, x);
}

template<typename E, typename T>
void easy_ref::set_handler() noexcept
{
	constexpr auto h = option::handler<E>::template from_static_fn<T>();
	static_assert(bool(h), "Could not extract `T::handle(e)`");
	h.easy(handle, nullptr);
}

template<typename E, typename T, typename D>
void easy_ref::set_handler(D *x) noexcept
{
	constexpr auto h = option::handler<E>::template from_static_fn<T, D>();
	static_assert(bool(h), "Could not extract `T::handle(e, d);`");
	h.easy(handle, x);
}
} // namespace curl

// defines easy_ref event types.
// required to go here to avoid circular dependency issue.
#include "curl++/easy_events.hpp"
#endif // CURLPLUSPLUS_EASY_REF_HPP
