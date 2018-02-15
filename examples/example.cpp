#include <iostream>
#include <atomic>
#include <thread>
#include <zmq.hpp>

using namespace std;

int main(int argc, char* argv[]) {

  if (argc != 1) {
    cout << "usage: something" << endl;
    return 1;
  }

  atomic<bool> quit(false);

  thread server_thread([&]() {
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (!quit) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(&request);
        std::cout << "[Server] Receieved request" << std::endl;

        //  Do some 'work'
        this_thread::sleep_for(chrono::milliseconds(1000));

        //  Send reply back to client
        zmq::message_t reply(5);
        memcpy(reply.data (), "World", 5);
        socket.send(reply);
        std::cout << "[Server] Sent reply" << std::endl;
    }
  });

  thread client_thread([&]() {
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    while (!quit) {
        const char* buf = "Hello";
        zmq::message_t request(strlen(buf));
        memcpy(request.data(), buf, strlen(buf));

        //  Wait for next request from client
        socket.send(request);
        std::cout << "[Client] Sent request" << std::endl;

        //  Do some 'work'
        this_thread::sleep_for(chrono::milliseconds(100));

        //  Send reply back to client
        zmq::message_t reply;
        socket.recv(&reply);
        cout << "[Client] Received reply" << endl;
    }
  });

  cin.get();
  quit = true;

  server_thread.join();
  client_thread.join();

  return 0;
}
