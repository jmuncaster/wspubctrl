add_executable(simple_server simple_server.cpp)
target_link_libraries(simple_server wspubctrl)

add_executable(simple_client simple_client.cpp)
target_link_libraries(simple_client wspubctrl)

add_executable(multi_server multi_server.cpp)
target_link_libraries(multi_server wspubctrl)

add_executable(multi_client multi_client.cpp)
target_link_libraries(multi_client wspubctrl)

