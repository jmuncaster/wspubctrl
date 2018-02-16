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
  string server_data = "";

  while (!server_quit) {
    // Check for ctrl request
    server.wait_for_request(0, [&](const string& request) {
      cout << "[Server " << get_ts() << "]  req: " << request << endl;
      server_data = "DATA(" + request + ")";
      string reply = server_data;
      cout << "[Server " << get_ts() << "]  rep: " << reply << endl;
      return reply;
    });

    // Do some 'work' and publish
    this_thread::sleep_for(milliseconds(100));
    cout << "[Server " << get_ts() << "] data: " << server_data << endl;
    server.publish_data(server_data);
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
  int iter = 0;
  zpubctrl::CtrlClient client;
  client.request(to_string(iter));
  while (!client_quit) {
    try {
      string request = to_string(++iter);
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

