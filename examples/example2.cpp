#include <iostream>
#include <atomic>
#include <thread>

#include "server.hpp"
#include "client.hpp"

using namespace std;
using namespace std::chrono;

atomic<bool> server_quit(false);
atomic<bool> client_quit(false);

string get_ts() {
  static auto epoch = steady_clock::now();
  auto dt = steady_clock::now() - epoch;
  return to_string(duration_cast<milliseconds>(dt).count()) + " ";
}

void server_entry() {
  zpubctrl::Server server;

  cout << "[Server] Waiting for start message..." << endl;
  server.wait_for_request();
  bool do_work = true;

  while (!server_quit) {
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
}

void sub_client_entry() {
  zpubctrl::SubClient client;
  while (!client_quit) {
    string data = client.wait_for_data();
    cout << "[Sub    " << get_ts() << "] data: " << data << endl;
  }
  cout << "[Sub    " << get_ts() << "] thrd: quit" << endl;
}

void ctrl_client_entry() {
  zpubctrl::CtrlClient client;
  client.request("start");
  while (!client_quit) {
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
}

int main(int argc, char* argv[]) {

  // Server process
  thread server_thread(server_entry);

  // Client process
  thread client_thread([]() {
    thread sub_client_thread(sub_client_entry);
    thread ctrl_client_thread(ctrl_client_entry);
    sub_client_thread.join();
    ctrl_client_thread.join();
  });

  cin.get(); // press enter

  cout << "main: quit" << endl;
  client_quit = true;
  server_quit = true;

  client_thread.join();
  server_thread.join();

  return 0;
}

