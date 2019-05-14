add_library(sanitize_address INTERFACE)
target_compile_options(sanitize_address INTERFACE -fsanitize=address)
target_link_options(sanitize_address INTERFACE -fsanitize=address)

add_library(sanitize_ub INTERFACE)
target_compile_options(sanitize_ub INTERFACE -fsanitize=undefined)
target_link_options(sanitize_ub INTERFACE -fsanitize=undefined)
