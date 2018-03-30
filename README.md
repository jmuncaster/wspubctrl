# Websockets Publisher with Control Socket

This library implements a simple network communication pattern. On the server-side, we implement a
simple class with a *publish* socket for streaming and a *request-reply* control-socket for synchronous
control messages. On the client-side we provide a simple class to *subscribe* to the published stream as
well as issue synchronous requests to the control socket.

The backend is based on [Simple-Websocket-Server](https://github.com/eidheim/Simple-WebSocket-Server).

## Getting Started

### Build

Dependencies:
- Boost
- OpenSSL

### Linux

Build with [cmake](cmake.org).

```bash
$ mkdir build && cd build
$ cmake -D BOOT_ROOT=/path/to/boost ..
$ cmake --build .
```

or, add this as a submodule or take a snapshot of the repository and in your CMakeLists.txt, add:

```CMake
add_subdirectory(path/to/wspubctrl wspubctrl)
add_executable(myapp main.cpp)
target_link_libraries(myapp wspubctrl)
```

### Windows

- OpenSSL: Install openssl to the default location. Installer [here](https://slproweb.com/products/Win32OpenSSL.html). I use [Win64 OpenSSL v1.1.0h](https://slproweb.com/download/Win64OpenSSL-1_1_0h.exe).
- Boost: See [boost.org](www.boost.org)

```bash
$ mkdir build && cd build
$ cmake -G "Visual Studio 14 2015 Win64" -D BOOST_ROOT='C:\path\to\boost' -D BUILD_EXAMPLES=ON -D BUILD_TESTS=ON ..
$ cmake --build .
```

### Example

See `examples` directory.

### Build Examples and Tests

To build the examples and tests run:
```bash
$ mkdir -p build && cd build
$ cmake -D BUILD_EXAMPLES=ON -D BUILD_TESTS=ON ..
$ make
```

