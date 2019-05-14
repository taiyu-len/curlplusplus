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
