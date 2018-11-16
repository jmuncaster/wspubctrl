#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  cout << "Start client. Press ENTER to issue commands to cycle through texts..." << endl;
  cout << " * control   : localhost:5554/ctrl" << endl;
  cout << " * subscribe : localhost:5554/pub" << endl;
  wspubctrl::CtrlClient ctrl_client("localhost:5554/ctrl");
  wspubctrl::SubClient sub_client("localhost:5554/pub");

  // Sub thread continuously reports the stream on the same line
  atomic<bool> quit(false);
  auto sub_thread = thread([&]() {
    try {
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

  try {
    vector<string> texts = {"Bonjour!", "Next we will try the empty string", "", "This is the last text"};

    int i = 0;
    while (true) {
      // Wait for "user input"
      string input;
      getline(cin, input);

      if (!input.empty() && input == "q") {
        break;
      }

      // Sends commands to server to cycle through these texts
      auto reply = ctrl_client.request(input, timeout_ms);
      if (reply != "OK") {
        cout << "\n==> ERROR: " << reply << endl;
        continue;
      }
      else {
        cout << "\e[1A" << flush; // go up a line
      }
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

