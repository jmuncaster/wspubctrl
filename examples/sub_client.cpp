#include <iostream>
#include <wspubctrl/client.hpp>

using namespace std;

const int timeout_ms = 5000;

int main(int argc, char* argv[]) {

  if (argc <= 1) {
    cout << "example: sub_client localhost:5554/pub" << endl;
    return 1;
  }

  string pub_uri = argv[1];

  try {
    cout << "Start sub client. Use ctrl client to issue 'quit' command to shut down subscribers." << endl;
    cout << " * subscribe: " << pub_uri << endl;
    wspubctrl::SubClient sub_client(pub_uri);

    cout << "Connecting to " << pub_uri << "..." << flush;
    sub_client.connect();
    cout << " connected." << endl;

    for (;;) {
      string data = sub_client.wait_for_data(timeout_ms);
      if (data == "quit") {
        break;
      }
      else {
        cout << "\r" << data << "\e[K" << flush;
      }
    }
    cout << endl;
  }
  catch (exception& e) {
    // probably a timeout
    cerr << "exception: " << e.what() << endl;
    return 1;
  }

  return 0;
}

