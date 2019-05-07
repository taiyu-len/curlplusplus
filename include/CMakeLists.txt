set_property(TARGET curl++ PROPERTY PUBLIC_HEADER
	curl++/buffer.hpp
	curl++/easy.hpp
	curl++/easy_handle.hpp
	curl++/easy_info.hpp
	curl++/easy_opt.hpp
	curl++/easy_ref.hpp
	curl++/extract_function.hpp
	curl++/global.hpp
	curl++/translate_type.hpp
	curl++/types.hpp
)
target_include_directories(curl++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
