#include <wspubctrl/server.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <algorithm>
#include <map>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  cout << "Start server" << endl;
  cout << "  * control on *:5554/ctrl" << endl;
  cout << "  * publish on *:5554/pub" << endl;
  wspubctrl::Server server(5554, "/ctrl");
  server.add_publish_endpoint("/pub");
  server.start();

  // Server greeting text
  string text = "Hello, World!";

  // Main program loop
  int iter = 0;
  while (true) {

    // Check for ctrl request to change the text
    server.wait_for_request(0, [&](const string& request) {
      cout << "got request: " << request << endl;
      if (request.empty()) {
        return "Cannot set empty text";
      }
      else {
        text = request;
        return "OK";
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

