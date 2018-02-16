cmake_minimum_required(VERSION 3.2.2)

project(zpubctrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(src)

# Examples and tests
option(BUILD_JWS_EXAMPLES "Build examples" ON)
if (BUILD_JWS_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_JWS_TESTS "Build tests" OFF)
if (BUILD_JWS_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
