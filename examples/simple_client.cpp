#include "client.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {

  cout << "Start client. Press ENTER to cycle through texts..." << endl;

  // Sub thread continuously reports the stream on the same line
  atomic<bool> quit(false);
  thread sub_thread([&]() {
    zpubctrl::SubClient sub_client;
    while (!quit) {
      auto data = sub_client.wait_for_data();
      cout << "\r" << data << "\e[K" << flush;
    }
  });

  cin.get();

  // Main loop cycles through texts in response to user input
  zpubctrl::CtrlClient ctrl_client;
  vector<string> texts = {"Bonjour!", "Next we will try the empty string", "", "This is the last text"};
  for (size_t i = 0; i < texts.size(); ++i) {
    auto reply = ctrl_client.request(texts[i % texts.size()]);
    if (reply != "OK") {
      cout << "\n==> ERROR: " << reply << endl;
      continue;
    }
    cin.get(); // press enter to cycle texts
    cout << "\e[1A" << flush; // go up a line
  }

  quit = true;
  sub_thread.join();

  return 0;
}

