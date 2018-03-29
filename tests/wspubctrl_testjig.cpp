#include <wspubctrl/server.hpp>
#include <wspubctrl/client.hpp>
#include <iostream>
#include <atomic>
#include <thread>

using namespace std;
using namespace std::chrono;

atomic<bool> server_quit(false);
atomic<bool> client_quit(false);

string get_ts() {
  static auto epoch = steady_clock::now();
  auto dt = steady_clock::now() - epoch;
  return to_string(duration_cast<milliseconds>(dt).count()) + " ";
}

// Main server process runs a program loop, publishes feed, and checks for control messages
void server_entry() {
  cout << "[Server " + get_ts() + "] Waiting for start message..." << endl;
  wspubctrl::Server server;
  string server_data;
  server.wait_for_request(-1, [&](const string& request) {
    server_data = "DATA(" + request + ")";
    return server_data;
  });

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

// Subscribe thread just listens to the feed
void sub_client_entry() {
  wspubctrl::SubClient client;
  while (!client_quit) {
    string data = client.wait_for_data(1000);
    if (!data.empty()) {
      cout << "[Sub    " << get_ts() << "] data: " << data << endl;
    }
    else {
      cout << "[Sub    " << get_ts() << "] timeout" << endl;
    }
  }
  cout << "[Sub    " << get_ts() << "] thrd: quit" << endl;
}

// Control thread issue commands every few seconds
void ctrl_client_entry() {
  int iter = 0;
  wspubctrl::CtrlClient client;
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

