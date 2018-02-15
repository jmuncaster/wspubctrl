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

      this_thread::sleep_for(chrono::milliseconds(77));
      server.publish_data("work");
    }
  });

  thread client_thread([&]() {
    simplezmq::Client client;

    int iter = 0;
    while (!quit) {
      if (iter % 10 == 0) {
        string reply = client.request("Hello");
        cout << "[Client] Received reply: " << reply << endl;
      }

      string data = client.wait_for_data();
      cout << "[Client] Got data: " << data << endl;
      ++iter;
    }
  });

  cin.get();
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}

