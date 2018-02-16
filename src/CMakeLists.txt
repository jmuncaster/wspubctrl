find_package(ZMQ REQUIRED)

add_library(zpubctrl
  constants.hpp
  ctrl_client.cpp
  ctrl_client.hpp
  server.cpp
  server.hpp
  sub_client.cpp
  sub_client.hpp
  simplezmq.cpp)

target_include_directories(zpubctrl PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(zpubctrl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zpubctrl ${ZMQ_LIBRARIES})
