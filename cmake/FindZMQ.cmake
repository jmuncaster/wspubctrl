find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY zmq)
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)

set(ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIR} ${CPPZMQ_INCLUDE_DIR})
set(ZMQ_LIBRARIES ${ZMQ_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZMQ DEFAULT_MSG ZMQ_INCLUDE_DIRS ZMQ_LIBRARIES)
