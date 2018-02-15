add_executable(test_jws test_jws.cpp)
target_link_libraries(test_jws jws)
add_test(NAME test_jws COMMAND test_jws --working_dir ${CMAKE_SOURCE_DIR})
