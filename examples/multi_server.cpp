#include <wspubctrl/server.hpp>
#include <algorithm>
#include <iostream>
#include <map>
#include <thread>

using namespace std;

// Main server process runs a program loop, publishes feed, and checks for control messages
int main(int argc, char** argv) {

  if (argc <= 1) {
    cout << "usage: multi_server <pub_path1> [pub_path2]..." << endl;
    cout << "example: multi_server /pub1 /pub2 /pub3" << endl;
    return 1;
  }

  // Create server and add all publish endpoints
  wspubctrl::Server server(5554, "/ctrl");
  cout << "Start server on port 5554" << endl;
  cout << "  * control on /ctrl" << endl;
  map<string, string> endpoint_texts;
  for (int i = 1; i < argc; ++i) {
    string endpoint = argv[i];
    cout << "  * publish on " << endpoint << endl;
    server.add_publish_endpoint(endpoint);
    endpoint_texts[endpoint] = "Hello, World"; // to be sent to this endpoint
  }

  // Main program loop
  server.start();
  int iter = 0;
  while (true) {
    // Check for ctrl request to change the text of a particular endpoint, or quit.
    server.wait_for_request(0, [&](const string& request) {
      cout << "got request: " << request << endl;

      // quit?
      if (request == "quit") {
        for (auto& p : endpoint_texts) {
          server.publish_data(p.first, "quit");
        }
        return "OK";
      }

      // format: <endpoint> [<text>|quit]
      auto it = request.find(" ");
      if (it == string::npos) {
        return "command format: quit|<endpoint> quit|<endpoint> <text>";
      }
      auto endpoint = request.substr(0, it);
      auto text = request.substr(it + 1, string::npos);

      if (!endpoint_texts.count(endpoint)) {
        return "unknown endpoint";
      }
      else if (text == "quit") {
        server.publish_data(endpoint, "quit");
        return "OK";
      }
      else {
        endpoint_texts[endpoint] = text;
        return "OK";
      }
    });

    // Do some 'work' mangling the texts
    this_thread::sleep_for(chrono::milliseconds(10));
    for (auto& p : endpoint_texts) {
      auto& endpoint = p.first;
      auto& text = p.second;
      string mangled_text = text;
      int j = ++iter % text.size();
      auto fn1 = (iter / text.size() % 2 == 0) ? ::toupper : ::tolower;
      auto fn2 = (iter / text.size() % 2 == 1) ? ::toupper : ::tolower;
      transform(text.begin(), text.begin() + j, mangled_text.begin(), fn1);
      transform(text.begin() + j, text.end(), mangled_text.begin() + j, fn2);

      // Publish
      server.publish_data(endpoint, mangled_text);
    }
  }
}

