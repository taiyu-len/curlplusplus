set_property(TARGET curl++ PROPERTY PUBLIC_HEADER
	curl++/buffer.hpp
	curl++/detail/extract_function.hpp
	curl++/easy.hpp
	curl++/easy_events.hpp
	curl++/easy_handle.hpp
	curl++/easy_info.hpp
	curl++/easy_opt.hpp
	curl++/easy_ref.hpp
	curl++/global.hpp
	curl++/multi.hpp
	curl++/option.hpp
	curl++/types.hpp
)
target_include_directories(curl++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
