cmake_minimum_required(VERSION 3.2.2)

project(zpubctrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(src)

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
