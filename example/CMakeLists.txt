string(APPEND CLANG_TIDY_CHECKS *) # all by default
string(APPEND CLANG_TIDY_CHECKS ,-readability-*)
string(APPEND CLANG_TIDY_CHECKS ,-fuchsia-*)
string(APPEND CLANG_TIDY_CHECKS ,-google-runtime-int)
string(APPEND CLANG_TIDY_CHECKS ,-hicpp-signed-bitwise) # allow bitflag macro
string(APPEND CLANG_TIDY_CHECKS ,-hicpp-vararg)  # allow using c functions
string(APPEND CLANG_TIDY_CHECKS ,-cppcoreguidelines-pro-type-vararg) # same
string(APPEND CLANG_TIDY_CHECKS ,-hicpp-no-array-decay) # allow __file__ macro
string(APPEND CLANG_TIDY_CHECKS ,-cppcoreguidelines-pro-bounds-array-to-pointer-decay) # same
string(APPEND CLANG_TIDY_CHECKS ,-cppcoreguidelines-pro-bounds-pointer-arithmetic) # allow argv[i]

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=${CLANG_TIDY_CHECKS}")

add_executable(simple simple.cc)
target_link_libraries(simple PRIVATE curl++)

add_executable(getinfo getinfo.cc)
target_link_libraries(getinfo PRIVATE curl++)

add_executable(url2file url2file.cc)
target_link_libraries(url2file PRIVATE curl++)
target_compile_features(url2file PRIVATE cxx_std_17)

add_executable(debug debug.cc)
target_link_libraries(debug PRIVATE curl++)
target_compile_features(debug PRIVATE cxx_std_17)

add_executable(use_test use_test.cpp)
target_link_libraries(use_test PRIVATE curl++)
target_compile_features(use_test PRIVATE cxx_std_17)

add_executable(time time.cc)
target_link_libraries(time PRIVATE curl++)

add_executable(10-at-a-time 10-at-a-time.cc)
target_link_libraries(10-at-a-time PRIVATE curl++)

add_executable(shared-connection-cache shared-connection-cache.cc)
target_link_libraries(shared-connection-cache PRIVATE curl++)
