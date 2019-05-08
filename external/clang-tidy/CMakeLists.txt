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
set_target_properties(curl++ PROPERTIES
	CXX_CLANG_TIDY "clang-tidy;-checks=${CMAKE_TIDY_CHECKS}")

