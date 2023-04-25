cmake_minimum_required(VERSION 3.2.2)

project(wspubctrl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# websockets backend
add_subdirectory(Simple-WebSocket-Server)
if (WIN32)
  target_compile_definitions(simple-websocket-server INTERFACE BOOST_ALL_NO_LIB)
endif()

# Prevent a build error with Boost 1.74+
target_compile_definitions(simple-websocket-server INTERFACE -DBOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT)

# wspubctrl library
add_library(wspubctrl
  wspubctrl/constants.hpp
  wspubctrl/client.hpp
  wspubctrl/ctrl_client.cpp
  wspubctrl/ctrl_client.hpp
  wspubctrl/server.cpp
  wspubctrl/server.hpp
  wspubctrl/sub_client.cpp
  wspubctrl/sub_client.hpp
  wspubctrl/detail/client_impl.cpp
  wspubctrl/detail/client_impl.hpp
  )
target_include_directories(wspubctrl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wspubctrl PUBLIC simple-websocket-server)

# Examples and tests
option(BUILD_EXAMPLES "Build examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_TESTS "Build tests" OFF)
if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

