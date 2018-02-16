add_executable(pubctrl_example pubctrl_example.cpp)
target_link_libraries(pubctrl_example zpubctrl pthread)

add_executable(simple_server simple_server.cpp)
target_link_libraries(simple_server zpubctrl)

add_executable(simple_client simple_client.cpp)
target_link_libraries(simple_client zpubctrl)
