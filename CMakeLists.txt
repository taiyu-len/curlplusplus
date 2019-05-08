cmake_minimum_required(VERSION 3.14)
project(curl++ VERSION 0.1)
set(IN_SOURCE_BUILD $<EQUAL:${CMAKE_SOURCE_DIR},${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(curl++ STATIC)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(external)

target_compile_features(curl++ PUBLIC cxx_std_14)
target_compile_options(curl++ PUBLIC -Wall)

target_link_libraries(curl++ PUBLIC curl)
target_link_libraries(curl++ PUBLIC sanitize_address)
target_include_directories(curl++ PUBLIC include)

IF(IN_SOURCE_BUILD)
add_subdirectory(example)
ENDIF()
