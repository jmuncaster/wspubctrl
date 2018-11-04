#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  // Sub thread continuously reports the stream on the same line
  atomic<bool> quit(false);
  thread sub_thread([&]() {
    try {
      wspubctrl::SubClient sub_client;
      sub_client.start();
      while (!quit) {
        auto data = sub_client.wait_for_data(timeout_ms);
        cout << "\r" << data << "\e[K" << flush;
      }
    }
    catch (exception& e) {
      // probably a timeout
      cerr << "sub thread: " << e.what() << endl;
      quit = true; // kill app on next iteration
    }
  });

  try {
    wspubctrl::CtrlClient ctrl_client;

    // Main loop cycles through texts in response to user input
    cout << "Start client. Press ENTER to issue commands to cycle through texts..." << endl;
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

  quit = true;
  sub_thread.join();
  cout << endl;

  return 0;
}

