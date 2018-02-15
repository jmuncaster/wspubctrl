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
      string request;
      if (server.get_request(request)) {
        cout << "[Server] Received request: " << request << endl;
        server.reply("World!");
      }
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
  });

  thread client_thread([&]() {
    simplezmq::Client client;

    while (!quit) {
      string reply = client.request("Hello");
      cout << "[Client] Received reply: " << reply << endl;
      this_thread::sleep_for(chrono::milliseconds(100));
    }
  });

  cin.get();
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}

