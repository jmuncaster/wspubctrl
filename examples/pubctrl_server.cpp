#include <wspubctrl/server.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <algorithm>
#include <map>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  // Start server with /ctrl and /pub
  wspubctrl::Server server(5554, "/ctrl");
  cout << "Start server on port 5554" << endl;
  cout << "  * control on /ctrl" << endl;
  cout << "  * publish on /pub" << endl;
  server.add_publish_endpoint("/pub");
  server.start();

  // Server greeting text
  string text = "Hello, World!";

  // Main program loop
  int iter = 0;
  while (true) {

    // Check for ctrl request to change the text or to kill all subscribers
    server.wait_for_request(0, [&](const string& request) {
      cout << "got request: " << request << endl;
      if (request.empty()) {
        return "Cannot set empty text";
      }
      else {
        if (request == "quit") {
          server.publish_data("/pub", "quit");
          return "OK";
        }
        else {
          text = request;
          return "OK";
        }
      }
    });

    // Do some 'work' mangling the texts and publish
    this_thread::sleep_for(chrono::milliseconds(10));
    string mangled_text = text;
    int j = ++iter % text.size();
    auto fn1 = (iter / text.size() % 2 == 0) ? ::toupper : ::tolower;
    auto fn2 = (iter / text.size() % 2 == 1) ? ::toupper : ::tolower;
    transform(text.begin(), text.begin() + j, mangled_text.begin(), fn1);
    transform(text.begin() + j, text.end(), mangled_text.begin() + j, fn2);

    server.publish_data("/pub", mangled_text);
  }
}

