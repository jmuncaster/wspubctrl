#include <iostream>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  if (argc <= 1) {
    cout << "example: ctrl_client localhost:5554/ctrl" << endl;
    return 1;
  }
  string ctrl_uri = argv[1];

  cout << "Start control client. Type text to send to server. 'quit' to exit." << endl;
  cout << " * control: " << ctrl_uri << endl;
  wspubctrl::CtrlClient ctrl_client(ctrl_uri);

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

