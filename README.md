# ZeroMQ Publisher with Control Socket

This library implements a simple network communication pattern. On the server-side, we implement a
simple class with a *publish* socket for streaming and a *request-reply* control-socket for synchronous
control messages. On the client-side we provide a simple class to *subscribe* to the published stream as
well as issue synchronous requests to the control socket.

The backend is implemented in [zeromq](http://zeromq.org) with a [C++ wrapper](https://github.com/zeromq/cppzmq).

## Getting Started

### Prerequisites

Add a snapshot of this repository to your source code or add it as a git submodule.

### Build

These instructions assume you are using [cmake](cmake.org) and you have installed ZeroMQ somewhere in your PATH.

In your CMakeLists.txt, add:
```CMake
add_subdirectory(path/to/zpubctrl zpubctrl)
add_executable(myapp main.cpp)
target_link_libraries(myapp zpubctrl)
```

### Example

Server:
```C++
#include <zpubctrl/server.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  zpubctrl::Server server;
  cout << "Start server" << endl;
  cout << "  * publish port: " << zpubctrl::default_data_port << endl;;
  cout << "  * control port: " << zpubctrl::default_ctrl_port << endl;;

  string text = "Hello World!";

  int iter = 0;
  while (true) {
    // Check for ctrl request to change the text
    server.wait_for_request(0, [&](const string& request) {
      if (request.empty()) {
        return "Cannot set empty text";
      }
      else {
        text = request;
        return "OK";
      }
    });

    // Do some 'work' mangling the text and publish
    string mangled_text = text;
    int i = ++iter % text.size();
    auto fn1 = (iter / text.size() % 2 == 0) ? ::toupper : ::tolower;
    auto fn2 = (iter / text.size() % 2 == 1) ? ::toupper : ::tolower;
    transform(text.begin(), text.begin() + i, mangled_text.begin(), fn1);
    transform(text.begin() + i, text.end(), mangled_text.begin() + i, fn2);
    this_thread::sleep_for(chrono::milliseconds(10));

    // Publish
    server.publish_data(mangled_text);
  }
}
```

Client:
```C++
#include <zpubctrl/client.hpp>
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  cout << "Start client. Press ENTER to cycle through texts..." << endl;

  // Sub thread continuously reports the stream on the same line
  atomic<bool> quit(false);
  thread sub_thread([&]() {
    try {
      zpubctrl::SubClient sub_client;
      while (!quit) {
        auto data = sub_client.wait_for_data(timeout_ms);
        cout << "\r" << data << "\e[K" << flush;
      }
    }
    catch (exception& e) {
      // probably a timeout
      cerr << e.what() << endl;
      quit = true; // kill app on next iteration
    }
  });

  cin.get();

  // Main loop cycles through texts in response to user input
  try {
    zpubctrl::CtrlClient ctrl_client;
    vector<string> texts = {"Bonjour!", "Next we will try the empty string", "", "This is the last text"};
    for (size_t i = 0; !quit && i < texts.size(); ++i) {
      auto reply = ctrl_client.request(texts[i % texts.size()], timeout_ms);
      if (reply != "OK") {
        cout << "\n==> ERROR: " << reply << endl;
        continue;
      }

      cin.get(); // press enter to cycle texts
      cout << "\e[1A" << flush; // go up a line
    }
  }
  catch (exception& e) {
    // probably a timeout
    cerr << e.what() << endl;
  }

  quit = true;
  sub_thread.join();

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

