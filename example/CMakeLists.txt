add_executable(simple simple.cc)
target_link_libraries(simple PRIVATE curl++)
add_executable(getinfo getinfo.cc)
target_link_libraries(getinfo PRIVATE curl++)
