#include <wspubctrl/server.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <algorithm>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  wspubctrl::Server server;
  cout << "Start server" << endl;
  cout << "  * publish on " << wspubctrl::default_pub_uri << endl;;
  cout << "  * control on " << wspubctrl::default_ctrl_uri << endl;;

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

