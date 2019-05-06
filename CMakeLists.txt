cmake_minimum_required(VERSION 3.14)
project(curl++ VERSION 0.1)

add_library(curl++ STATIC)
add_subdirectory(src)
add_subdirectory(include)
target_compile_options(curl++ PUBLIC -fsanitize=address)
target_link_options(curl++ PUBLIC -fsanitize=address)
target_compile_options(curl++ PUBLIC -std=c++14)
target_compile_options(curl++ PUBLIC -Wall)
target_link_libraries(curl++ PUBLIC curl)
target_include_directories(curl++ PUBLIC include)

add_subdirectory(example)
