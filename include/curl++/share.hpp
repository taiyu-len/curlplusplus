#ifndef CURLPLUSPLUS_SHARE_HPP
#define CURLPLUSPLUS_SHARE_HPP
#include "extract_function.hpp"
#include <curl/curl.h>
#include <utility>
namespace curl {

struct share_ref : detail::set_handler_base<share_ref> {
protected:
	CURLSH* _handle = nullptr;
public:
	// share events.
	struct lock;
	struct unlock;

	/**
	 * Construct empty handle by default
	 */
	share_ref() noexcept = default;

	/**
	 * Construct from given raw handle.
	 */
	share_ref(CURLSH *handle) noexcept
	: handle(h)
	{};

	/**
	 * @returns true iff handle is valid
	 */
	explicit operator bool() const noexcept
	{
		return _handle != nullptr;
	}

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
	 * Sets options using types defined in share_opt.hpp
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<CURLSHoption o, typename T>
	void set(option::detail::share_option<o, T> x)
	{
		invoke(::curl_share_setopt, _handle, o, x.value);
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
		invoke(::curl_easy_setopt, _handle, o, x);
	}
};

struct share : share_ref
{
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

} // namespace curl
#endif // CURLPLUSPLUS_SHARE_HPP
