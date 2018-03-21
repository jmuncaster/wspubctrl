cmake_minimum_required(VERSION 3.2.2)

project(zpubctrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(Simple-WebSocket-Server)

# zpubctrl library
find_package(ZMQ REQUIRED)
add_library(zpubctrl
  zpubctrl/constants.hpp
  zpubctrl/client.hpp
  zpubctrl/ctrl_client.cpp
  zpubctrl/ctrl_client.hpp
  zpubctrl/server.cpp
  zpubctrl/server.hpp
  zpubctrl/sub_client.cpp
  zpubctrl/sub_client.hpp)
target_include_directories(zpubctrl PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(zpubctrl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zpubctrl PUBLIC ${ZMQ_LIBRARIES} simple-websocket-server )

# Examples and tests
option(BUILD_ZPUBCTRL_EXAMPLES "Build examples" OFF)
if (BUILD_ZPUBCTRL_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_ZPUBCTRL_TESTS "Build tests" OFF)
if (BUILD_ZPUBCTRL_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

