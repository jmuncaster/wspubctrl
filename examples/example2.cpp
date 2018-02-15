#include <iostream>
#include <atomic>
#include <thread>

#include "simplezmq.hpp"

using namespace std;

int main(int argc, char* argv[]) {

  if (argc != 1) {
    cout << "usage: something" << endl;
    return 1;
  }

  atomic<bool> quit(false);

  thread server_thread([&]() {
    simplezmq::Server server;

    while (!quit) {
      server.check_for_request([&](const string& request) {
        cout << "[Server] Received request: " << request << endl;
        return "World!";
      });
      cout << "[Server] work..." << endl;
      this_thread::sleep_for(chrono::milliseconds(77));
    }
  });

  thread client_thread([&]() {
    simplezmq::Client client;

    while (!quit) {
      cout << "[Client] Sent request" << endl;
      string reply = client.request("Hello");
      cout << "[Client] Received reply: " << reply << endl;
      cout << "[Client] work..." << endl;
      this_thread::sleep_for(chrono::milliseconds(500));
    }
  });

  cin.get();
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}

