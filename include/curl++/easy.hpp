#ifndef CURLPLUSPLUS_EASY_HPP
#define CURLPLUSPLUS_EASY_HPP
#include "extract_function.hpp"
#include "handle_base.hpp"
#include "info.hpp"
#include "invoke.hpp"
#include "option.hpp"
#include "types.hpp"

#include <chrono>
#include <cstddef>           // for size_t
#include <curl/curl.h>       // for CURL, curl_easy_*, etc
#include <stdexcept>         // for exceptions
#include <utility>           // for exchange
namespace curl {

/**
 * A light weight non-owning handle for a curl easy request.
 */
struct easy_ref
	: detail::handle_base<CURL*>
	, detail::set_handler_base<easy_ref>
{
	/**
	 * Flag type used in pause(pause_flags).
	 */
	struct pause_t {
		unsigned long value;
		constexpr auto operator|(pause_t x) const noexcept -> pause_t
		{
			return { value | x.value };
		}
		constexpr auto operator|=(pause_t x) noexcept -> pause_t&
		{
			value |= x.value; return *this;
		}
	};

	/**
	 * Contains pause flag values.
	 */
	struct pause_flag {
		static constexpr pause_t recv = {CURLPAUSE_RECV};
		static constexpr pause_t send = {CURLPAUSE_SEND};
		static constexpr pause_t all  = {CURLPAUSE_ALL};
		static constexpr pause_t cont = {CURLPAUSE_CONT};
	};

	/**
	 * Flags used in setopt(netrc).
	 */
	enum class netrc : unsigned long {
		optional = CURL_NETRC_OPTIONAL,
		ignored  = CURL_NETRC_IGNORED,
		required = CURL_NETRC_REQUIRED
	};

	/**
	 * Flags used in setopt(httpauth).
	 */
	enum class httpauth : unsigned long {
		basic     = CURLAUTH_BASIC,
		digest    = CURLAUTH_DIGEST,
		digest_ie = CURLAUTH_DIGEST_IE,
		bearer    = CURLAUTH_BEARER,
		negotiate = CURLAUTH_NEGOTIATE,
		ntlm      = CURLAUTH_NTLM,
		ntlm_wb   = CURLAUTH_NTLM_WB,
		any       = CURLAUTH_ANY,
		anysafe   = CURLAUTH_ANYSAFE,
		only      = CURLAUTH_ONLY,
	};

	// events that can emitted by CURL
	struct debug;
	struct header;
	struct read;
	struct seek;
	struct write;
	struct progress;

	using detail::handle_base<CURL*>::handle_base;

	/**
	 * cleanup existing handle and set to given raw handle.
	 */
	void reset(CURL* new_handle = nullptr) noexcept
	{
		::curl_easy_cleanup(std::exchange(_handle, new_handle));
	}

	/**
	 * resets existing handle or creates a new handle.
	 *
	 * @throws std::runtime_error on failure to create handle.
	 */
	void init()
	{
		if (_handle != nullptr) {
			::curl_easy_reset(_handle);
		} else {
			_handle = ::curl_easy_init();
		}
		if (_handle == nullptr) {
			throw std::runtime_error("failed to initialize easy handle");
		}
	}

	/**
	 * see curl_easy_pause().
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void pause(pause_t flags)
	{
		invoke(::curl_easy_pause, _handle, flags.value);
	}

	/**
	 * see curl_easy_perform().
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	void perform()
	{
		invoke(::curl_easy_perform, _handle);
	}

	/**
	 * see curl_easy_setopt.
	 * set option manually.
	 *
	 * @throws curl::code
	 * @pre *this
	 */
	template<typename T>
	void setopt(CURLoption o, T x)
	{
		invoke(::curl_easy_setopt, _handle, o, x);
	}

	/**
	 * Macro to define a function of the form NAME(TYPE) that sets that
	 * particular option.
	 */
#define SETOPT_FUNC(NAME, OPTION, TYPE) \
	void NAME(detail::option<TYPE> x) { setopt(CURLOPT_ ## OPTION, x); }

	/**
	 * Macro to define a function taking an enumflag of the form
	 * setopt(FLAG) that sets that particlar option
	 */
#define SETFLAG_FUNC(OPTION, TYPE) \
	void setopt(TYPE x) { \
		setopt(CURLOPT_ ## OPTION, static_cast<unsigned long>(x)); \
	}

	SETOPT_FUNC(url             , URL            , std::string);
	SETOPT_FUNC(verbose         , VERBOSE        , bool);
	SETOPT_FUNC(no_progress     , NOPROGRESS     , bool);
	SETOPT_FUNC(follow_location , FOLLOWLOCATION , bool);
	SETOPT_FUNC(error_buffer    , ERRORBUFFER    , error_buffer);
	SETOPT_FUNC(share           , SHARE          , detail::handle_base<CURLSH*>);

	SETFLAG_FUNC(NETRC   , netrc);
	SETFLAG_FUNC(HTTPAUTH, httpauth);

	/**
	 * see CURLOPT_PRIVATE.
	 */
	template<typename T>
	void userdata(T* x)
	{
		// store as void* using C style cast.
		setopt(CURLOPT_PRIVATE, (void*)(x));
	}

#undef  SETOPT_FUNC

	/**
	 * Macro to generate getinfo function for easy handles.
	 */
#define GETINFO_FUNC(NAME, OPTION, ...) \
	auto NAME() const -> __VA_ARGS__ { \
		return detail::info<__VA_ARGS__>::getinfo(_handle, CURLINFO_ ## OPTION); \
	}

	GETINFO_FUNC(url                     , EFFECTIVE_URL            , std::string);
	// TODO make response code type that can stringify the response and whatnot.
	GETINFO_FUNC(response_code           , RESPONSE_CODE            , long);
	GETINFO_FUNC(http_connect_code       , HTTP_CONNECTCODE         , long);
	GETINFO_FUNC(http_version            , HTTP_VERSION             , long);

	GETINFO_FUNC(redirect_count          , REDIRECT_COUNT           , long);
	GETINFO_FUNC(redirect_url            , REDIRECT_URL             , std::string);
	// bytes
	GETINFO_FUNC(size_upload             , SIZE_UPLOAD_T            , curl_off_t);
	GETINFO_FUNC(size_download           , SIZE_DOWNLOAD_T          , curl_off_t);
	// bytes per second
	GETINFO_FUNC(speed_upload            , SPEED_UPLOAD_T           , curl_off_t);
	GETINFO_FUNC(speed_download          , SPEED_DOWNLOAD_T         , curl_off_t);
	GETINFO_FUNC(header_size             , HEADER_SIZE              , long);
	GETINFO_FUNC(request_size            , REQUEST_SIZE             , long);
	// Bytes
	GETINFO_FUNC(content_length_download , CONTENT_LENGTH_DOWNLOAD_T, curl_off_t);
	GETINFO_FUNC(content_length_upload   , CONTENT_LENGTH_UPLOAD_T  , curl_off_t);
	GETINFO_FUNC(content_type            , CONTENT_TYPE             , std::string);

	// T should be a pointer.
	template<typename T>
	GETINFO_FUNC( userdata               , PRIVATE
	            , std::enable_if_t<std::is_pointer<T>::value, T>);
	// TODO rather then a duration, a timepoint may be better
	GETINFO_FUNC(filetime                , FILETIME_T               , std::chrono::seconds);
	// Timings
	GETINFO_FUNC(total_time              , TOTAL_TIME_T             , std::chrono::microseconds);
	GETINFO_FUNC(namelookup_time         , NAMELOOKUP_TIME_T        , std::chrono::microseconds);
	GETINFO_FUNC(connect_time            , CONNECT_TIME_T           , std::chrono::microseconds);
	GETINFO_FUNC(appconnect_time         , APPCONNECT_TIME_T        , std::chrono::microseconds);
	GETINFO_FUNC(pretransfer_time        , PRETRANSFER_TIME_T       , std::chrono::microseconds);
	GETINFO_FUNC(starttransfer_time      , STARTTRANSFER_TIME_T     , std::chrono::microseconds);
	GETINFO_FUNC(redirect_time           , REDIRECT_TIME_T          , std::chrono::microseconds);
#undef GETINFO_FUNC
};

/**
 * Lightweight RAII wrapper for a curl easy handle.
 */
struct easy : public easy_ref {
	/**
	 * Construct with valid handle.
	 *
	 * @throws std::runtime_error
	 */
	easy()
	{
		init();
	}

	/**
	 * Take ownership from an easy_ref.
	 *
	 * @warning Beware multiple owners.
	 */
	explicit easy(easy_ref er) noexcept
	: easy_ref(er)
	{};

	easy(easy const&) = delete;
	auto operator=(easy const&) -> easy& = delete;

	/**
	 * Transfer ownership from given handle to this one.
	 */
	easy(easy&& x) noexcept
	: easy_ref(std::exchange(x._handle, nullptr))
	{}

	/**
	 * Transfer ownership from given handle to this one.
	 * Cleans up existing handle.
	 */
	auto operator=(easy&& x) noexcept -> easy&
	{
		reset(std::exchange(x._handle, nullptr));
		return *this;
	}

	/**
	 * Cleanup handle.
	 */
	~easy() noexcept
	{
		reset();
	}

	/**
	 * Release owner ship of easy handle and return non-owning handle.
	 */
	auto release() noexcept -> easy_ref
	{
		return std::exchange(_handle, nullptr);
	}
private:
	// hide raw handle from inheriting types.
	using easy_ref::_handle;
};

/**
 * CRTP base type that automatically sets callbacks and data ptrs.
 *
 * @param T The parent type.
 */
template<typename T>
struct easy_base : public easy {
	/**
	 * Sets up handlers for events to use member functions from parent
	 * class, or do nothing if there is no handler.
	 */
	easy_base() noexcept
	{
		set_handler< debug,    true >(self());
		set_handler< header,   true >(self());
		set_handler< read,     true >(self());
		set_handler< seek,     true >(self());
		set_handler< write,    true >(self());
		set_handler< progress, true >(self());
	}
private:
	auto self() noexcept -> T*
	{
		return static_cast<T*>(this);
	}

	using easy::easy;

protected:
	using easy::set_handler;
};

} // namespace curl
#include "easy_events.hpp"
#endif // CURLPLUSPLUS_EASY_HPP
