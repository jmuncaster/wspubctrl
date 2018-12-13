add_library(wspubctrl
  constants.hpp
  client.hpp
  ctrl_client.cpp
  ctrl_client.hpp
  server.cpp
  server.hpp
  sub_client.cpp
  sub_client.hpp
  detail/client_detail.cpp
  detail/client_detail.hpp
  asdf
  )

asdf;ask;dljfa;lsdk
target_include_directories(wspubctrl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wspubctrl)
