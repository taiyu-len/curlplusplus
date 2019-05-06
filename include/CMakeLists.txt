set_property(TARGET curl++ PROPERTY PUBLIC_HEADER
	curl++/easy.hpp
	curl++/easy_event.hpp
	curl++/easy_handle.hpp
	curl++/easy_info.hpp
	curl++/easy_opt.hpp
	curl++/easy_ref.hpp
	curl++/global.hpp
	curl++/types.hpp
	curl++/util.hpp
)
target_include_directories(curl++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
