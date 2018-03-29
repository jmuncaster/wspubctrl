cmake_minimum_required(VERSION 3.2.2)

project(wspubctrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# websockets backend
add_subdirectory(Simple-WebSocket-Server)

# wspubctrl library
find_package(ZMQ REQUIRED)
add_library(wspubctrl
  wspubctrl/constants.hpp
  wspubctrl/client.hpp
  wspubctrl/ctrl_client.cpp
  wspubctrl/ctrl_client.hpp
  wspubctrl/server.cpp
  wspubctrl/server.hpp
  wspubctrl/sub_client.cpp
  wspubctrl/sub_client.hpp)
target_include_directories(wspubctrl PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(wspubctrl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wspubctrl PUBLIC ${ZMQ_LIBRARIES} simple-websocket-server )

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

