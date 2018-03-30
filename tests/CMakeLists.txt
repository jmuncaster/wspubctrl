find_package(Threads)
add_executable(wspubctrl_testjig wspubctrl_testjig.cpp)
target_link_libraries(wspubctrl_testjig wspubctrl ${CMAKE_THREAD_LIBS_INIT})
