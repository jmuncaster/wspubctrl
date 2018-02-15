#include "simplezmq.hpp"

#include <vector>

using namespace std;

namespace simplezmq {
  Server::Server(int pub_port, int ctrl_port) :
    _context(),
    _pub_socket(_context, ZMQ_PUB),
    _reply_socket(_context, ZMQ_REP) {

    _pub_socket.bind("tcp://*:" + to_string(pub_port));
    _reply_socket.bind("tcp://*:" + to_string(ctrl_port));
  }

  bool Server::check_for_request(function<string(const string&)> handler) {
    vector<zmq::pollitem_t> items {{(void*)_reply_socket, 0, ZMQ_POLLIN, 0}};
    if (zmq::poll(items, 0)) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t msg;
        _reply_socket.recv(&msg);
        string request_payload(static_cast<char*>(msg.data()), msg.size());
        string reply_payload = handler(request_payload);
        _reply_socket.send(reply_payload.data(), reply_payload.size());
        return true;
      }
    }

    return false;
  }

  void Server::publish(const string& payload) {
    _pub_socket.send(payload.data(), payload.size());
  }

  Client::Client(const string& server_address, int sub_port, int ctrl_port) :
    _context(),
    _sub_socket(_context, ZMQ_SUB),
    _request_socket(_context, ZMQ_REQ) {

    _sub_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0); // Accept everything
    _sub_socket.setsockopt(ZMQ_CONFLATE, 1);      // Drop old messages

    _sub_socket.connect("tcp://" + server_address + ":" + to_string(sub_port));
    _request_socket.connect("tcp://" + server_address + ":" + to_string(ctrl_port));
  }

  string Client::request(const string& payload) {
    _request_socket.send(payload.data(), payload.size());
    zmq::message_t msg;
    _request_socket.recv(&msg);
    return string(static_cast<char*>(msg.data()), msg.size());
  }

  string Client::wait_for_data(int timeout_ms) {
    vector<zmq::pollitem_t> items {{(void*)_sub_socket, 0, ZMQ_POLLIN, 0}};
    if (zmq::poll(items, timeout_ms)) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t msg;
        _sub_socket.recv(&msg);
        string payload(static_cast<char*>(msg.data()), msg.size());
        return payload;
      }
    }
    return {};
  }
}

