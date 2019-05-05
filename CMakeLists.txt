cmake_minimum_required(VERSION 3.14)
project(curl++ VERSION 0.1)

add_library(curl++ STATIC)
add_subdirectory(src)
add_subdirectory(include)

target_compile_options(curl++ PUBLIC -std=c++14)
target_compile_options(curl++ PRIVATE -Wall)
target_link_libraries(curl++ PUBLIC curl)
