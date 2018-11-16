#include <iostream>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  cout << "Start client. Type text to send to server. 'quit' to exit." << endl;
  cout << " * control   : localhost:5554/ctrl" << endl;
  wspubctrl::CtrlClient ctrl_client("localhost:5554/ctrl");

  try {
    string input;
    while (input != "quit") {
      getline(cin, input);

      auto reply = ctrl_client.request(input, timeout_ms);

      cout << "Server replied: " << reply << endl;
    }
  }
  catch (exception& e) {
    // probably a timeout
    cerr << e.what() << endl;
  }

  return 0;
}

