#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  if (argc <= 2) {
    cout << "usage: simple_client host port [pub_path]" << endl;
    return 1;
  }

  string host = argv[1];
  string port = argv[2];

  // Sub thread continuously reports the stream on the same line
  atomic<bool> quit(false);
  thread sub_thread;
  if (argc >= 4) {
    string pub_uri  = host + ":" + port + argv[3];
    cout << "subscribe: " << pub_uri << endl;
    sub_thread = thread([&]() {
      try {
        wspubctrl::SubClient sub_client(pub_uri);
        sub_client.start();
        while (!quit) {
          auto data = sub_client.wait_for_data(timeout_ms);
          cout << "\r" << data << "\e[K" << flush;
        }
        cout << endl;
      }
      catch (exception& e) {
        // probably a timeout
        cerr << "sub thread: " << e.what() << endl;
        quit = true; // kill app on next iteration
      }
    });
  }

  // Ctrl loop
  try {
    // Main loop cycles through texts in response to user input
    string ctrl_uri = host + ":" + port + "/ctrl";
    wspubctrl::CtrlClient ctrl_client(ctrl_uri);
    cout << "Start client. Press ENTER to issue commands to cycle through texts..." << endl;
    cout << "  control: " << ctrl_uri << endl;
    cin.get();
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

  if (argc >= 4) {
    quit = true;
    sub_thread.join();
  }

  return 0;
}

