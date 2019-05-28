cmake_minimum_required(VERSION 3.14)
project(curl++ VERSION 0.1)
set(IN_SOURCE_BUILD $<EQUAL:${CMAKE_SOURCE_DIR},${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(curl++ INTERFACE)
add_subdirectory(include)
add_subdirectory(external)

target_compile_features(curl++ INTERFACE cxx_std_14)
target_compile_options(curl++  INTERFACE -Wall -Wextra)

target_link_libraries(curl++ INTERFACE curl)
target_include_directories(curl++ INTERFACE include)

IF(IN_SOURCE_BUILD)
target_link_libraries(curl++ INTERFACE sanitize_address)
add_subdirectory(example)
ENDIF()
