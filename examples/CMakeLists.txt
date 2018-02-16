add_executable(example example.cpp)
target_link_libraries(example zpubctrl pthread)

add_executable(example2 example2.cpp)
target_link_libraries(example2 zpubctrl pthread)
