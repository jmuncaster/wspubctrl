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
      bool got_request = server.wait_for_request(0, [&](const string& request) {
        cout << "[Server " << get_ts() << "]  req: " << request << endl;
        do_work = !do_work;
        string reply = get_ts() + (do_work ? "do_work is true" : "do_work is false");
        cout << "[Server " << get_ts() << "]  rep: " << reply << endl;
        return reply;
      });

      // Do some 'work'
      this_thread::sleep_for(milliseconds(100));
      string data = get_ts() + (do_work ? "work" : "no-op");
      cout << "[Server " << get_ts() << "] data: " << data << endl;

      server.publish_data(data);
    }
    cout << "[Server " << get_ts() << "] thrd: quit" << endl;
  });

  this_thread::sleep_for(seconds(1));

  thread client_thread([&]() {

    thread sub_thread([&]() {
      simplezmq::SubClient client;
      while (!quit) {
        string data = client.wait_for_data();
        cout << "[Sub    " << get_ts() << "] data: " << data << endl;
      }
      cout << "[Sub    " << get_ts() << "] thrd: quit" << endl;
    });

    thread ctrl_thread([&]() {
      simplezmq::CtrlClient client;
      client.request("start");
      while (!quit) {
        try {
          string request = get_ts() + "toggle do_work";
          cout << "[Ctrl   " << get_ts() << "]  req: " << request << endl;
          string reply = client.request(request);
          cout << "[Ctrl   " << get_ts() << "]  rep: " << reply << endl;
          this_thread::sleep_for(milliseconds(1000));
        }
        catch (exception& e) {
          cout << "[Ctrl   " << get_ts() << "]  err: " << e.what() << endl;
        }
      }
      cout << "[Ctrl   " << get_ts() << "] thrd: quit" << endl;
    });

    sub_thread.join();
    ctrl_thread.join();
    cout << "[Client " << get_ts() << "] thrd: quit" << endl;
  });

  cin.get();

  cout << "main: quit" << endl;
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}

