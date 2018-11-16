#include <wspubctrl/server.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <algorithm>
#include <map>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  if (argc <= 1) {
    cout << "usage: simple_server <pub_path1> [pub_path2]..." << endl;
    return 1;
  }

  map<string, string> publish_texts;

  wspubctrl::Server server;
  cout << "Start server" << endl;
  cout << "  * control on " << wspubctrl::default_ctrl_uri << endl;;
  for (int i = 1; i < argc; ++i) {
    string path = argv[i];
    cout << "  * publish on " << wspubctrl::default_host << ":" << wspubctrl::default_port << path << endl;
    server.add_publish_endpoint(path);
    publish_texts[path] = "Hello, World!";
  }
  server.start();

  int iter = 0;
  while (true) {
    // Check for ctrl request to change the text
    server.wait_for_request(0, [&](const string& request) {
      cout << "got request: " << request << endl;
      if (request.empty()) {
        return "Cannot set empty text";
      }
      else {
        for (int i = 1; i < argc; ++i) {
          publish_texts[argv[i]] = request;
        }
        return "OK";
      }
    });

    // Do some 'work' mangling the texts and publish
    this_thread::sleep_for(chrono::milliseconds(10));
    for (int i = 1; i < argc; ++i) {
      string path = argv[i];
      auto& text = publish_texts[path];
      string mangled_text = text;
      int j = ++iter % text.size();
      auto fn1 = (iter / text.size() % 2 == 0) ? ::toupper : ::tolower;
      auto fn2 = (iter / text.size() % 2 == 1) ? ::toupper : ::tolower;
      transform(text.begin(), text.begin() + j, mangled_text.begin(), fn1);
      transform(text.begin() + j, text.end(), mangled_text.begin() + j, fn2);

      server.publish_data(path, mangled_text);
    }
  }
}

