#ifndef CURLPLUSPLUS_HANDLE_BASE_HPP
#define CURLPLUSPLUS_HANDLE_BASE_HPP
namespace curl {
namespace detail {

template<typename Handle>
struct handle_base {
protected:
	Handle _handle = nullptr;
public:
	/**
	 * Default construct empty handle.
	 */
	handle_base() noexcept = default;

	/**
	 * Construct from raw handle.
	 */
	handle_base(Handle x) noexcept
	: _handle(x)
	{}

	/**
	 * @returns true iff handle is valid.
	 */
	explicit operator bool() const noexcept
	{
		return _handle != nullptr;
	}

	/**
	 * Returns raw handle.
	 */
	auto raw() -> Handle
	{
		return _handle;
	}
};

} // namespace detail
} // namespace curl
#endif // CURLPLUSPLUS_HANDLE_BASE_HPP
