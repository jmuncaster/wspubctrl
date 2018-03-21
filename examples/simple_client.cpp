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
      thread sub_client_thread([&]() {
        sub_client.start();
      });
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
    thread ctrl_client_thread([&]() {
      ctrl_client.start();
    });
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

