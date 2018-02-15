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

    cout << "[Server] Waiting to be started..." << endl;
    server.wait_for_request();

    while (!quit) {

      // Check for ctrl request
      server.wait_for_request(0, [&](const string& request) {
        cout << "[Server] Received request: " << request << endl;
        return "World!";
      });

      // Do some 'work'
      this_thread::sleep_for(chrono::milliseconds(77));
      server.publish_data("work");
    }
  });

  this_thread::sleep_for(chrono::seconds(1));

  thread client_thread([&]() {
    simplezmq::Client client;

    client.request("Start");

    int iter = 1;
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

