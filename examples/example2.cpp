#include <iostream>
#include <atomic>
#include <thread>

#include "simplezmq.hpp"

using namespace std;
using namespace std::chrono;

steady_clock::time_point epoch = steady_clock::now();

string get_ts() {
  auto dt = steady_clock::now() - epoch;
  return to_string(duration_cast<milliseconds>(dt).count()) + " ";
}

int main(int argc, char* argv[]) {

  if (argc != 1) {
    cout << "usage: something" << endl;
    return 1;
  }

  atomic<bool> quit(false);

  thread server_thread([&]() {
    simplezmq::Server server;

    cout << "[Server] Waiting for start message..." << endl;
    server.wait_for_request();
    bool do_work = true;

    while (!quit) {
      // Check for ctrl request
      server.wait_for_request(0, [&](const string& request) {
        cout << "[Server]  req: " << request << endl;
        do_work = !do_work;
        return get_ts() + (do_work ? "do_work is true" : "do_work is false");
      });

      // Do some 'work'
      string result = get_ts() + (do_work ? "work" : "no-op");
      this_thread::sleep_for(milliseconds(100));

      server.publish_data(result);
    }
  });

  this_thread::sleep_for(seconds(1));

  thread client_thread([&]() {
    simplezmq::Client client;

    client.request("start");
    auto last_toggle = steady_clock::now();

    while (!quit) {
      string data = client.wait_for_data();
      cout << "[Client] data: " << data << endl;

      auto time_since_toggle = steady_clock::now() - last_toggle;
      if (duration_cast<seconds>(time_since_toggle).count() >= 1) {
        string reply = client.request(get_ts() + "toggle do_work");
        cout << "[Client]  rep: " << reply << endl;
        last_toggle = steady_clock::now();
      }
    }
  });

  cin.get();
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}

