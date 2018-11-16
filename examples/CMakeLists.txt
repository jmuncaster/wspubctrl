add_executable(ctrl_client ctrl_client.cpp)
target_link_libraries(ctrl_client wspubctrl)

add_executable(sub_client sub_client.cpp)
target_link_libraries(sub_client wspubctrl)

add_executable(pubctrl_server pubctrl_server.cpp)
target_link_libraries(pubctrl_server wspubctrl)

add_executable(multipubctrl_server multipubctrl_server.cpp)
target_link_libraries(multipubctrl_server wspubctrl)

