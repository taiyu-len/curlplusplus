#ifndef CURLPLUSPLUS_SHARE_HPP
#define CURLPLUSPLUS_SHARE_HPP
#include "extract_function.hpp"
#include "handle_base.hpp"
#include <curl/curl.h>
#include <utility>
namespace curl {

struct share_ref
	: detail::handle_base<CURLSH*>
	, detail::set_handler_base<share_ref>
{
	using set_handler_base = detail::set_handler_base<share_ref>;

	/**
	 * see curl_lock_data in curl.h.
	 * determines what type of data is shared/being accessed.
	 */
	enum lock_data {
		cookie      = CURL_LOCK_DATA_COOKIE,
		dns         = CURL_LOCK_DATA_DNS,
		ssl_session = CURL_LOCK_DATA_SSL_SESSION,
		connect     = CURL_LOCK_DATA_CONNECT,
		psl         = CURL_LOCK_DATA_PSL
	};

	/**
	 * see curl_lock_access in curl.h.
	 * determines whether we are reading or writing, if using a
	 * reader/writer mutex.
	 */
	enum lock_access {
		shared = CURL_LOCK_ACCESS_SHARED,
		single = CURL_LOCK_ACCESS_SINGLE
	};

	// events.
	struct lock;
	struct unlock;

	using  detail::handle_base<CURLSH*>::handle_base;

	/**
	 * cleanup existing handle and set to given raw handle.
	 */
	void reset(CURLSH* new_handle = nullptr) noexcept
	{
		::curl_share_cleanup(std::exchange(_handle, new_handle));
	}

	/**
	 * resets existing handle or creates a new handle.
	 *
	 * @throws std::runtime_error on failure to create handle.
	 */
	void init()
	{
		reset(curl_share_init());
		if (_handle == nullptr)
		{
			throw std::runtime_error("failed to initialize easy handle");
		}
	}

	/**
	 * see curl_share_setopt().
	 * sets what data to share.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void share_data(lock_data data)
	{
		invoke(::curl_share_setopt, _handle, CURLSHOPT_SHARE, data);
	}

	/**
	 * see curl_share_setopt().
	 * sets what data to not share.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void unshare_data(lock_data data)
	{
		invoke(::curl_share_setopt, _handle, CURLSHOPT_UNSHARE, data);
	}

	/**
	 * set lock/unlock handler using member functions of T.
	 */
	template<typename T>
	void set_handler(T* x) noexcept
	{
		set_handler_base::set_handler< lock   >(x);
		set_handler_base::set_handler< unlock >(x);
	}

	/**
	 * set lock/unlock handler using static member functions of T.
	 */
	template<typename T>
	void set_handler() noexcept
	{
		set_handler_base::set_handler< lock,   T >();
		set_handler_base::set_handler< unlock, T >();
	}

	/**
	 * set lock/unlock handler using static member functions of T taking
	 * custom data.
	 */
	template<typename T, typename D>
	void set_handler(D *x) noexcept
	{
		set_handler_base::set_handler< lock,   T >(x);
		set_handler_base::set_handler< unlock, T >(x);
	}

	/**
	 * see curl_share_setopt.
	 * set option manually.
	 *
	 * @warning Not type safe. avoid this
	 * @throws curl::code
	 * @pre *this
	 */
	template<typename T>
	void set(CURLSHoption o, T x)
	{
		invoke(::curl_share_setopt, _handle, o, x);
	}
};

struct share : share_ref
{
	/**
	 * Default construct with a valid handle.
	 *
	 * @throws std::runtime_error
	 */
	share()
	{
		init();
	}

	/**
	 * Take ownership from a share_ref.
	 *
	 * @warning Beware multiple owners.
	 */
	explicit share(share_ref sr) noexcept
	: share_ref(sr)
	{}

	share(share const&) = delete;
	auto operator=(share const&) -> easy& = delete;

	/**
	 * Transfer ownership from given handle to this one.
	 */
	share(share&& x) noexcept
	: share_ref(std::exchange(x._handle, nullptr))
	{}

	/**
	 * Transfer ownership from given handle to this handle.
	 * Cleans up existing handle.
	 */

	auto operator=(share&& x) noexcept -> share&
	{
		reset(std::exchange(x._handle, nullptr));
		return *this;
	}

	/**
	 * cleanup handle.
	 */
	~share() noexcept
	{
		reset();
	}

	/**
	 * Release ownership over handle and return non-owning handle.
	 */
	auto release() noexcept -> share_ref
	{
		return std::exchange(_handle, nullptr);
	}

private:
	using share_ref::_handle;
};

/**
 * CRTP base class that automatically sets callbacks and data ptrs.
 *
 * @param T The parent type.
 */
template<typename T>
struct share_base : public share {
	/**
	 * Sets up handlers for events based on what T can handle.
	 */
	share_base() noexcept
	{
		set_handler(self());
	}

private:
	auto self() noexcept -> T*
	{
		return static_cast<T*>(this);
	}
	using share::share;

protected:
	using share::set_handler;
};

} // namespace curl

#include "share_events.hpp"
#endif // CURLPLUSPLUS_SHARE_HPP
