# ZeroMQ Publisher with Constrol Socket

This library implements a simple network communication pattern. On the server-side, we implement a
simple class with a *publish* socket for streaming and a *request-reply* control-socket for synchronous
control messages. On the client-side we provide a simple class to *subscribe* to the published stream as
well as issue synchronous requests to the control socket.

The backend is implemented in [zeromq](http://zeromq.org) with a [C++ wrapper](https://github.com/zeromq/cppzmq).

## Getting Started

### Prerequisites

Add a snapshot of this repository to your source code or add it as a git submodule.

### Build

These instructions assume you are using [cmake](cmake.org).

In your CMakeLists.txt, add:
```CMake
add_subdirectory(path/to/zpubctrl jws)
add_executable(myapp main.cpp)
target_link_libraries(myapp jws)
```

### Example

Server:
```C++
#include <zpubctrl/server.hpp>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

  zpubctrl::Server server;
  int iter = 0;
  while (true) {
    server.wait_for_request(0, [&](const string& request) {
      echo_reply = request;
      return "OK";
    });
  }

  

  auto validator = load_validator(argv[1]);
  auto document = load_json(argv[2]);
  validator.validate(document); // throws on error

  cout << "document validated" << endl;

  return 0;
}
```

### Build Examples and Tests

To build the examples and tests run:
```bash
$ mkdir -p build && cd build
$ cmake -D BUILD_EXAMPLES=ON -D BUILD_TESTS=ON ..
$ make
```

### Run Tests
```bash
./test_jws --working_dir /path/to/jws
```
